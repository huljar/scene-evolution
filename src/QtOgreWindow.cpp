#include <scene-evolution/QtOgreWindow.h>
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
#include <Compositor/OgreCompositorManager2.h>
#endif

/*
Note that we pass any supplied QWindow parent to the base QWindow class. This is necessary should we
need to use our class within a container.
*/
QTOgreWindow::QTOgreWindow(QWindow *parent)
    : QWindow(parent)
    , mRoot(nullptr)
    , mWindow(nullptr)
    , mCamera(nullptr)
    , mCameraMan(nullptr)
    , mResourcesCfg(Ogre::StringUtil::BLANK)
    , mPluginsCfg(Ogre::StringUtil::BLANK)
    , mUpdatePending(false)
    , mAnimating(false)
{
    setAnimating(true);
    installEventFilter(this);
    mBackground = Ogre::ColourValue(0.0f, 0.0f, 0.0f);
}

/*
Upon destruction of the QWindow object we destroy the Ogre3D scene.
*/
QTOgreWindow::~QTOgreWindow()
{
    delete mCameraMan;
    delete mRoot;
}

/*
In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
class in any way we inform Qt that they will be unused.
*/
void QTOgreWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

/*
Our initialization function. Called by our renderNow() function once when the window is first exposed.
*/
void QTOgreWindow::initialize()
{
    /*
    As shown Ogre3D is initialized normally; just like in other documentation.
    */
    mResourcesCfg = "../config/resources.cfg";
    mPluginsCfg = "../config/plugins.cfg";

    mRoot = new Ogre::Root(mPluginsCfg);

    /*
    Parse resources configuration file
    */
    Ogre::ConfigFile ogreConfig;
    ogreConfig.load(mResourcesCfg);

    Ogre::String name, locType;
    Ogre::ConfigFile::SectionIterator secIt = ogreConfig.getSectionIterator();
    while(secIt.hasMoreElements()) {
        Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator it;
        for(it = settings->begin(); it != settings->end(); ++it) {
            locType = it->first;
            name = it->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
        }
    }

    /*
    This list sets up the search order for used render system.
    */
    const Ogre::RenderSystemList& rsList = mRoot->getAvailableRenderers();
    Ogre::RenderSystem* rs = nullptr;

    Ogre::StringVector renderOrder;
#if defined(Q_OS_WIN)
    renderOrder.push_back("Direct3D9");
    renderOrder.push_back("Direct3D11");
#endif
    renderOrder.push_back("OpenGL");
    renderOrder.push_back("OpenGL 3+");
    for (Ogre::StringVector::iterator iter = renderOrder.begin(); iter != renderOrder.end(); iter++)
    {
        for (Ogre::RenderSystemList::const_iterator it = rsList.begin(); it != rsList.end(); it++)
        {
            if ((*it)->getName().find(*iter) != Ogre::String::npos)
            {
                rs = *it;
                break;
            }
        }
        if (rs != nullptr) break;
    }
    if (rs == nullptr)
    {
        if (!mRoot->restoreConfig())
        {
            if (!mRoot->showConfigDialog())
                OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                    "Abort render system configuration",
                    "QTOgreWindow::initialize");
        }
    }

    // Call preInit setup function which may be overridden in subclasses
    preInitSetup();

    /*
    Setting size and VSync on windows will solve a lot of problems
    */
    QString dimensions = QString("%1 x %2").arg(this->width()).arg(this->height());
    rs->setConfigOption("Video Mode", dimensions.toStdString());
    rs->setConfigOption("Full Screen", "No");
    rs->setConfigOption("VSync", "Yes");
    mRoot->setRenderSystem(rs);
    mRoot->initialise(false);

    Ogre::NameValuePairList parameters;
    /*
    Flag within the parameters set so that Ogre3D initializes an OpenGL context on it's own.
    */
    if (rs->getName().find("GL") <= rs->getName().size())
        parameters["currentGLContext"] = Ogre::String("false");

    /*
    We need to supply the low level OS window handle to this QWindow so that Ogre3D knows where to draw
    the scene. Below is a cross-platform method on how to do this.
    If you set both options (externalWindowHandle and parentWindowHandle) this code will work with OpenGL
    and DirectX.
    */
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    parameters["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)(this->winId()));
    parameters["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)(this->winId()));
#else
    parameters["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
    parameters["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
#endif

#if defined(Q_OS_MAC)
    parameters["macAPI"] = "cocoa";
    parameters["macAPICocoaUseNSView"] = "true";
#endif

    /*
    Note below that we supply the creation function for the Ogre3D window the width and height
    from the current QWindow object using the "this" pointer.
    */
    mWindow = mRoot->createRenderWindow("Qt Window",
        this->width(),
        this->height(),
        false,
        &parameters);
    mWindow->setVisible(true);

    /*
    The rest of the code in the initialization function is standard Ogre3D scene code. Consult other
    tutorials for specifics.
    */
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    const size_t numThreads = std::max<int>(1, Ogre::PlatformInformation::getNumLogicalCores());
    Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
    if (numThreads > 1)threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, numThreads, threadedCullingMethod);
#else
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
#endif

    createCamera();

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    createCompositor();
#else
    Ogre::Viewport* pViewPort = mWindow->addViewport(mCamera);
    pViewPort->setBackgroundColour(mBackground);
#endif

    mCamera->setAspectRatio(
        Ogre::Real(mWindow->getWidth()) / Ogre::Real(mWindow->getHeight()));
    mCamera->setAutoAspectRatio(true);

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    createScene();

    mRoot->addFrameListener(this);

    // Call postInit setup which may be overridden by subclasses
    postInitSetup();

    emit initialized();
}

void QTOgreWindow::createCamera() {
    mCamera = mSceneMgr->createCamera("MainCamera");
    mCamera->setPosition(Ogre::Vector3(0.0f, 300.0f, 500.0f));
    mCamera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    mCamera->setNearClipDistance(5.0f);
    //mCamera->setFarClipDistance(200.0f);
    mCameraMan = new OgreQtBites::SdkQtCameraMan(mCamera);   // create a default camera controller
}

void QTOgreWindow::createScene()
{
    /*
    Example scene
    Derive this class for your own purpose and overwrite this function to have a working Ogre widget with
    your own content.
    */
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    Ogre::Entity* sphereMesh = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
#else
    Ogre::Entity* sphereMesh = mSceneMgr->createEntity("mySphere", Ogre::SceneManager::PT_SPHERE);
#endif

    Ogre::SceneNode* childSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    childSceneNode->attachObject(sphereMesh);

    Ogre::MaterialPtr sphereMaterial = Ogre::MaterialManager::getSingleton().create("SphereMaterial",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);

    sphereMaterial->getTechnique(0)->getPass(0)->setAmbient(0.1f, 0.1f, 0.1f);
    sphereMaterial->getTechnique(0)->getPass(0)->setDiffuse(0.2f, 0.2f, 0.2f, 1.0f);
    sphereMaterial->getTechnique(0)->getPass(0)->setSpecular(0.9f, 0.9f, 0.9f, 1.0f);
    //sphereMaterial->setAmbient(0.2f, 0.2f, 0.5f);
    //sphereMaterial->setSelfIllumination(0.2f, 0.2f, 0.1f);

    sphereMesh->setMaterialName("SphereMaterial");
    childSceneNode->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    childSceneNode->setScale(Ogre::Vector3(0.01f, 0.01f, 0.01f)); // Radius, in theory.

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    Ogre::SceneNode* pLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light = mSceneMgr->createLight();
    pLightNode->attachObject(light);
    pLightNode->setPosition(20.0f, 80.0f, 50.0f);
#else
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setPosition(20.0f, 80.0f, 50.0f);
#endif
}

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
void QTOgreWindow::createCompositor()
{
    /*
    Example compositor
    Derive this class for your own purpose and overwite this function to have a working Ogre
    widget with your own compositor.
    */
    Ogre::CompositorManager2* compMan = mRoot->getCompositorManager2();
    const Ogre::String workspaceName = "default scene workspace";
    const Ogre::IdString workspaceNameHash = workspaceName;
    compMan->createBasicWorkspaceDef(workspaceName, mBackground);
    compMan->addWorkspace(mSceneMgr, mWindow, mCamera, workspaceNameHash, true);
}
#endif

void QTOgreWindow::render()
{
    /*
    How we tied in the render function for OGre3D with QWindow's render function. This is what gets call
    repeatedly. Note that we don't call this function directly; rather we use the renderNow() function
    to call this method as we don't want to render the Ogre3D scene unless everything is set up first.
    That is what renderNow() does.

    Theoretically you can have one function that does this check but from my experience it seems better
    to keep things separate and keep the render function as simple as possible.
    */
    Ogre::WindowEventUtilities::messagePump();
    mRoot->renderOneFrame();
}

void QTOgreWindow::renderLater()
{
    /*
    This function forces QWindow to keep rendering. Omitting this causes the renderNow() function to
    only get called when the window is resized, moved, etc. as opposed to all of the time; which is
    generally what we need.
    */
    if (!mUpdatePending)
    {
        mUpdatePending = true;
        QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool QTOgreWindow::event(QEvent *event)
{
    /*
    QWindow's "message pump". The base method that handles all QWindow events. As you will see there
    are other methods that actually process the keyboard/other events of Qt and the underlying OS.

    Note that we call the renderNow() function which checks to see if everything is initialized, etc.
    before calling the render() function.
    */

    switch (event->type())
    {
    case QEvent::UpdateRequest:
        mUpdatePending = false;
        renderNow();
        return true;

    default:
        return QWindow::event(event);
    }
}

/*
Called after the QWindow is reopened or when the QWindow is first opened.
*/
void QTOgreWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

/*
The renderNow() function calls the initialize() function when needed and if the QWindow is already
initialized and prepped calls the render() method.
*/
void QTOgreWindow::renderNow()
{
    if (!isExposed())
        return;

    if (mRoot == nullptr)
    {
        initialize();
    }

    render();

    if (mAnimating)
        renderLater();
}

/*
Our event filter; handles the resizing of the QWindow. When the size of the QWindow changes note the
call to the Ogre3D window and camera. This keeps the Ogre3D scene looking correct.
*/
bool QTOgreWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == this)
    {
        if (event->type() == QEvent::Resize)
        {
            if (isExposed() && mWindow != nullptr)
            {
                mWindow->resize(this->width(), this->height());
            }
        }
    }

    return false;
}

void QTOgreWindow::preInitSetup() {

}

void QTOgreWindow::postInitSetup() {

}

/*
How we handle keyboard and mouse events.
*/
void QTOgreWindow::keyPressEvent(QKeyEvent* e)
{
    if(mCameraMan)
        mCameraMan->injectKeyDown(*e);
}

void QTOgreWindow::keyReleaseEvent(QKeyEvent* e)
{
    if(mCameraMan)
        mCameraMan->injectKeyUp(*e);
}

void QTOgreWindow::mouseMoveEvent(QMouseEvent* e)
{
    static int lastX = e->x();
    static int lastY = e->y();
    int relX = e->x() - lastX;
    int relY = e->y() - lastY;
    lastX = e->x();
    lastY = e->y();

    if(mCameraMan && (e->buttons() & Qt::LeftButton))
        mCameraMan->injectMouseMove(relX, relY);
}

void QTOgreWindow::wheelEvent(QWheelEvent* e)
{
    if(mCameraMan)
        mCameraMan->injectWheelMove(*e);
}

void QTOgreWindow::mousePressEvent(QMouseEvent* e)
{
    if(mCameraMan)
        mCameraMan->injectMouseDown(*e);
}

void QTOgreWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if(mCameraMan)
        mCameraMan->injectMouseUp(*e);

    QPoint pos = e->pos();
    Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
        (Ogre::Real)pos.x() / mWindow->getWidth(),
        (Ogre::Real)pos.y() / mWindow->getHeight());
    Ogre::RaySceneQuery* pSceneQuery = mSceneMgr->createRayQuery(mouseRay);
    pSceneQuery->setSortByDistance(true);
    Ogre::RaySceneQueryResult vResult = pSceneQuery->execute();
    for (size_t ui = 0; ui < vResult.size(); ui++)
    {
        if (vResult[ui].movable)
        {
            if (vResult[ui].movable->getMovableType().compare("Entity") == 0)
            {
                emit entitySelected((Ogre::Entity*)vResult[ui].movable);
            }
        }
    }
    mSceneMgr->destroyQuery(pSceneQuery);
}

/*
Function to keep track of when we should and shouldn't redraw the window; we wouldn't want to do
rendering when the QWindow is minimized. This takes care of those scenarios.
*/
void QTOgreWindow::setAnimating(bool animating)
{
    mAnimating = animating;

    if (animating)
        renderLater();
}

Ogre::Root* QTOgreWindow::getOgreRoot() const {
    return mRoot;
}

Ogre::SceneManager* QTOgreWindow::getOgreSceneManager() const {
    return mSceneMgr;
}

Ogre::RenderWindow* QTOgreWindow::getOgreRenderWindow() const {
    return mWindow;
}

Ogre::Camera* QTOgreWindow::getOgreCamera() const {
    return mCamera;
}

bool QTOgreWindow::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    mCameraMan->frameRenderingQueued(evt);
    return true;
}

void QTOgreWindow::log(Ogre::String msg)
{
    if(Ogre::LogManager::getSingletonPtr() != nullptr) Ogre::LogManager::getSingletonPtr()->logMessage(msg);
}

void QTOgreWindow::log(QString msg)
{
    log(Ogre::String(msg.toStdString().c_str()));
}
