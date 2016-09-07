#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/interop.h>

#include <algorithm>

using namespace interop;

SceneObjectManager::SceneObjectManager(const Scene& currentScene, unsigned int currentSceneIdx, RGBDScene* currentRGBDScene, bool showBoundingBoxes)
    : mCurrentSceneIdx(currentSceneIdx)
    , mSceneMgr(currentRGBDScene->getSceneMgr())
    , mShowBoundingBoxes(showBoundingBoxes)
{
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(currentRGBDScene->getManualObject());
    mSceneInfoMap.emplace(currentSceneIdx, std::make_tuple(currentScene, currentRGBDScene, node, cv::Mat1b::zeros(currentScene.getLabelImg().size()), false));
}

SceneObjectManager::~SceneObjectManager() {
    for(auto&& pair : mSceneObjectsMap) {
        for(auto&& obj : pair.second) {
            obj.second->detachAllObjects();
            mSceneMgr->destroySceneNode(obj.second);
        }
    }
    mSceneObjectsMap.clear(); // Clearing manually to destroy shared pointers before deleting RGBDScenes

    for(auto&& pair : mSceneInfoMap) {
        delete std::get<1>(pair.second);
        mSceneMgr->destroySceneNode(std::get<2>(pair.second));
    }
    mSceneInfoMap.clear();
}

bool SceneObjectManager::registerObject(const SceneObjPtr& obj) {
    return registerObject(obj, mCurrentSceneIdx);
}

bool SceneObjectManager::registerObject(const SceneObjPtr& obj, unsigned int sceneIdx) {
    std::cout << "Registering object: " << obj->getName().toStdString() << std::endl;

    // Check if the object has a mesh or if the object is already registered
    ObjVec& vec = mSceneObjectsMap[sceneIdx];
    if(!obj->hasManualObject() || findObject(obj, sceneIdx) != vec.end()) {
        std::cerr << "Object " << obj->getName().toStdString() << " has no manual object or it is already registered." << std::endl;
        return false;
    }

    // Add object and scene node to vector
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    vec.emplace_back(obj, node);

    // Attach object if visible
    if(obj->getVisible())
        node->attachObject(obj->getManualObject());

    return true;
}

void SceneObjectManager::cutObject(const SceneObjPtr& obj) {
    cutObject(obj, mCurrentSceneIdx);
}

void SceneObjectManager::cutObject(const SceneObjPtr& obj, unsigned int sceneIdx) {
    // Create mask from object outline
    addToMask(obj->getOriginalPixels(), sceneIdx);
}

void SceneObjectManager::cutObjects(const std::vector<SceneObjPtr>& objs) {
    cutObjects(objs, mCurrentSceneIdx);
}

void SceneObjectManager::cutObjects(const std::vector<SceneObjPtr>& objs, unsigned int sceneIdx) {
    // Build mask from all objects
    for(std::vector<SceneObjPtr>::const_iterator it = objs.begin(); it != objs.end(); ++it) {
        cutObject(*it, sceneIdx);
    }
}

void SceneObjectManager::updateObjects() {
    // Iterate over registered objects and their scene nodes
    for(auto&& pair : mSceneObjectsMap[mCurrentSceneIdx]) {
        // Update translation
        pair.second->setPosition(cvToOgre(pair.first->getCurrentTranslation()));

        // Update rotation
        pair.second->setOrientation(getQuaternion(pair.first->getCurrentRotation()));

        // Update visibility
        Ogre::ManualObject* obj = pair.first->getManualObject();
        if(pair.first->getVisible() && !obj->isAttached())
            pair.second->attachObject(obj);
        else if(!pair.first->getVisible() && obj->isAttached())
            pair.second->detachObject(obj);

        // Update bounding box display
        pair.second->showBoundingBox(mShowBoundingBoxes);
    }

    // Update background scene
    RGBDScene* rgbdScene;
    cv::Mat1b mask;
    bool needsUpdate;

    std::tie(std::ignore, rgbdScene, std::ignore, mask, needsUpdate) = mSceneInfoMap.at(mCurrentSceneIdx);

    if(needsUpdate)
        rgbdScene->meshify(mask, false);
}

bool SceneObjectManager::checkObjectInScene(const SEL::SceneObject& obj) const {
    return checkObjectInScene(obj, mCurrentSceneIdx);
}

bool SceneObjectManager::checkObjectInScene(const SEL::SceneObject& obj, unsigned int sceneIdx) const {
    SceneInfoMap::const_iterator sceneInfo = mSceneInfoMap.find(sceneIdx);
    if(sceneInfo == mSceneInfoMap.cend())
        return false;

    cv::Mat1b pixels = obj.getOriginalPixels();
    cv::Mat1b mask = std::get<3>(sceneInfo->second);
    if(pixels.rows != mask.rows || pixels.cols != mask.cols)
        return false;

    unsigned int numThere = 0;
    unsigned int numCut = 0;

    for(cv::Mat1b::iterator it = pixels.begin(); it != pixels.end(); ++it) {
        if(*it == 255) {
            if(mask(it.pos()) == 255) ++numCut;
            else ++numThere;
        }
    }

    return numThere > numCut;
}

RGBDScene* SceneObjectManager::getRGBDScene() const {
    return std::get<1>(mSceneInfoMap.at(mCurrentSceneIdx));
}

RGBDScene* SceneObjectManager::getRGBDScene(unsigned int sceneIdx) const {
    SceneInfoMap::const_iterator sceneInfo = mSceneInfoMap.find(sceneIdx);
    if(sceneInfo == mSceneInfoMap.cend())
        return nullptr;
    return std::get<1>(sceneInfo->second);
}

Ogre::SceneNode* SceneObjectManager::getRGBDSceneNode() const {
    return std::get<2>(mSceneInfoMap.at(mCurrentSceneIdx));
}

Ogre::SceneNode* SceneObjectManager::getRGBDSceneNode(unsigned int sceneIdx) const {
    SceneInfoMap::const_iterator sceneInfo = mSceneInfoMap.find(sceneIdx);
    if(sceneInfo == mSceneInfoMap.cend())
        return nullptr;
    return std::get<2>(sceneInfo->second);
}

SceneObjectManager::ObjVec SceneObjectManager::getRegisteredObjects() const {
    SceneObjectsMap::const_iterator objs = mSceneObjectsMap.find(mCurrentSceneIdx);
    if(objs == mSceneObjectsMap.cend())
        return ObjVec();
    return objs->second;
}

bool SceneObjectManager::getShowBoundingBoxes() const {
    return mShowBoundingBoxes;
}

void SceneObjectManager::setShowBoundingBoxes(bool showBoundingBoxes) {
    mShowBoundingBoxes = showBoundingBoxes;
    updateObjects();
}

void SceneObjectManager::onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs& e) {
    Q_UNUSED(e);

    // Clear scene objects
    for(auto&& pair : mSceneObjectsMap) {
        for(auto&& obj : pair.second) {
            obj.second->detachAllObjects();
            mSceneMgr->destroySceneNode(obj.second);
        }
    }
    mSceneObjectsMap.clear();

    // Clear scene info
    for(auto&& pair : mSceneInfoMap) {
        delete std::get<1>(pair.second);
    }
    mSceneInfoMap.clear();
}

void SceneObjectManager::onSceneChanged(SceneChangedEventArgs& e) {
    // Hide objects of current scene
    // Check if they exist first (in case the dataset was changed)
    SceneInfoMap::iterator sceneInfo = mSceneInfoMap.find(mCurrentSceneIdx);
    if(sceneInfo != mSceneInfoMap.end()) {
        for(auto&& pair : mSceneObjectsMap) {
            for(auto&& obj : pair.second) {
                obj.second->detachObject(obj.first->getManualObject());
            }
        }
    }

    // Hide current RGBD scene
    std::get<1>(mSceneInfoMap.at(mCurrentSceneIdx))->getManualObject()->detachFromParent();

    // Change scene
    mCurrentSceneIdx = e.sceneIdx;

    // Show new RGBD scene, insert map entry for it if necessary
    sceneInfo = mSceneInfoMap.find(mCurrentSceneIdx);
    if(sceneInfo == mSceneInfoMap.end()) {
        // New entry
        Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(e.rgbdScene->getManualObject());
        mSceneInfoMap.emplace(mCurrentSceneIdx, std::make_tuple(e.scene, e.rgbdScene, node, cv::Mat1b::zeros(e.scene.getLabelImg().size()), false));
    }
    else if(std::get<1>(sceneInfo->second) == nullptr) {
        // Complete missing information (entry exists only for mask storage)
        std::get<0>(sceneInfo->second) = e.scene;
        std::get<1>(sceneInfo->second) = e.rgbdScene;

        Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(e.rgbdScene->getManualObject());
        std::get<2>(sceneInfo->second) = node;
    }
    else {
        // Just display
        std::get<2>(sceneInfo->second)->attachObject(std::get<1>(sceneInfo->second)->getManualObject());
    }

    // Show objects of new scene
    updateObjects();
}

void SceneObjectManager::addToMask(const cv::Mat1b& mask) {
    addToMask(mask, mCurrentSceneIdx);
}

void SceneObjectManager::addToMask(const cv::Mat1b& mask, unsigned int sceneIdx) {
    SceneInfoMap::iterator sceneInfo = mSceneInfoMap.find(sceneIdx);

    // Check if a scene with this index already exists, if not create a stub for it
    if(sceneInfo == mSceneInfoMap.end())
        sceneInfo = mSceneInfoMap.emplace(sceneIdx, std::make_tuple(Scene(), nullptr, nullptr,
                                                                    cv::Mat1b::zeros(std::get<0>(mSceneInfoMap.at(mCurrentSceneIdx)).getLabelImg().size()), true
                                                                    )).first;

    cv::Mat1b& currentMask = std::get<3>(sceneInfo->second);
    if(mask.rows != currentMask.rows || mask.cols != currentMask.cols)
        return;

    // Apply mask addition
    for(cv::Mat1b::const_iterator it = mask.begin(); it != mask.end(); ++it)
        if(*it == 255) currentMask(it.pos()) = 255;

    // Mark this scene to require an update
    std::get<4>(sceneInfo->second) = true;
}

SceneObjectManager::ObjVec::iterator SceneObjectManager::findObject(const SceneObjPtr& obj) {
    return findObject(obj, mCurrentSceneIdx);
}

SceneObjectManager::ObjVec::iterator SceneObjectManager::findObject(const SceneObjPtr& obj, unsigned int sceneIdx) {
    ObjVec& vec = mSceneObjectsMap[sceneIdx];
    for(ObjVec::iterator it = vec.begin(); it != vec.end(); ++it) {
        if(obj == it->first) return it;
    }
    return vec.end();
}
