#include <scene-evolution/SceneObjectManager.h>

SceneObjectManager::SceneObjectManager(RGBDScene* rgbdScene)
    : mRGBDScene(rgbdScene)
    , mCurrentMask(cv::Mat_<unsigned char>::zeros(rgbdScene->getDepthImage().size()))
{
}

SceneObjectManager::~SceneObjectManager() {
    Ogre::SceneManager* sceneMgr = mRGBDScene->getSceneMgr();
    for(std::vector<Ogre::SceneNode*>::iterator it = mSceneNodes.begin(); it != mSceneNodes.end(); ++it) {
        (*it)->detachAllObjects();
        sceneMgr->destroySceneNode(*it);
    }
}

bool SceneObjectManager::registerObject(const std::shared_ptr<SEL::SceneObject>& obj) {
    std::cout << "Registering object: " << obj->getName().toStdString() << std::endl;

    // Check if the object has a mesh or if the object is already attached to a scene node
    if(!obj->hasManualObject() || obj->getManualObject()->isAttached()) {
        std::cerr << "Object " << obj->getName().toStdString() << " has no manual object or it is already attached to a scene node." << std::endl;
        return false;
    }

    // Add to vector
    mSceneObjects.push_back(obj);

    // Add scene node for it
    Ogre::SceneNode* node = mRGBDScene->getSceneMgr()->getRootSceneNode()->createChildSceneNode();
    mSceneNodes.push_back(node);

    // Attach object
    node->attachObject(obj->getManualObject());

    return true;
}

void SceneObjectManager::cutObject(const std::shared_ptr<SEL::SceneObject>& obj) {
    // Create mask from object outline
    addToMask(obj->getOriginalPixels());
}

void SceneObjectManager::cutObjects(const std::vector<std::shared_ptr<SEL::SceneObject>>& objs) {
    // Build mask from all objects
    for(std::vector<std::shared_ptr<SEL::SceneObject>>::const_iterator it = objs.begin(); it != objs.end(); ++it) {
        cutObject(*it);
    }
}

void SceneObjectManager::updateObjects() {
    for(size_t i = 0; i < mSceneObjects.size(); ++i) {
        cv::Vec3f trans = mSceneObjects[i]->getCurrentTranslation();
        cv::Matx33f rot = mSceneObjects[i]->getCurrentRotation();

        mSceneNodes[i]->setPosition(Ogre::Vector3(trans[0], trans[1], trans[2]));

        mSceneNodes[i]->setOrientation(getQuaternion(rot));
    }

    mRGBDScene->meshify(mCurrentMask, false);
}

bool SceneObjectManager::checkObjectInScene(const SEL::SceneObject& obj) const {
    cv::Mat_<unsigned char> pixels = obj.getOriginalPixels();
    if(pixels.rows != mCurrentMask.rows || pixels.cols != mCurrentMask.cols)
        return false;

    unsigned int numThere = 0;
    unsigned int numCut = 0;

    for(cv::Mat_<unsigned char>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
        if(*it == 255) {
            if(mCurrentMask(it.pos()) == 255) ++numCut;
            else ++numThere;
        }
    }

    return numThere > numCut;
}

RGBDScene* SceneObjectManager::getRGBDScene() const {
    return mRGBDScene;
}

std::vector<std::shared_ptr<SEL::SceneObject>> SceneObjectManager::getRegisteredObjects() const {
    return mSceneObjects;
}

void SceneObjectManager::addToMask(const cv::Mat_<unsigned char>& mask) {
    if(mask.rows != mCurrentMask.rows || mask.cols != mCurrentMask.cols)
        return;

    for(cv::Mat_<unsigned char>::const_iterator it = mask.begin(); it != mask.end(); ++it)
        if(*it == 255) mCurrentMask(it.pos()) = 255;
}

Ogre::Quaternion SceneObjectManager::getQuaternion(const cv::Matx33f rotMat) const {
    return Ogre::Quaternion(Ogre::Matrix3(rotMat(0, 0), rotMat(0, 1), rotMat(0, 2),
                                          rotMat(1, 0), rotMat(1, 1), rotMat(1, 2),
                                          rotMat(2, 0), rotMat(2, 1), rotMat(2, 2)));
}
