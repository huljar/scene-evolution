#include <scene-evolution/CustomOgreWindow.h>
#include <scene-evolution/util.h>

CustomOgreWindow::CustomOgreWindow(QWindow *parent)
    : QTOgreWindow(parent)
{
}

CustomOgreWindow::~CustomOgreWindow() {

}

void CustomOgreWindow::createCamera() {
    mCamera = mSceneMgr->createCamera("MainCamera");
    mCamera->setPosition(Constants::InitialCameraPosition);
    mCamera->lookAt(Constants::InitialCameraLookAt);
    mCamera->setNearClipDistance(Constants::CameraNearClipDistance);
    mCamera->setFOVy(Ogre::Degree(55));

    mCameraMan = new OgreQtBites::SdkQtCameraMan(mCamera);   // create a default camera controller
    mCameraMan->setTopSpeed(150);
}

void CustomOgreWindow::keyPressEvent(QKeyEvent* e) {
    if(e->key() == Qt::Key_Space) {
        mCamera->setPosition(Constants::InitialCameraPosition);
        mCamera->lookAt(Constants::InitialCameraLookAt);
    }

    QTOgreWindow::keyPressEvent(e);
}

void CustomOgreWindow::createScene() {

}
