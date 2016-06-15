#include <scene-evolution/BoundingBoxManager.h>

BoundingBoxManager::BoundingBoxManager(Ogre::SceneManager* sceneMgr, unsigned int currentSceneIdx, const QString& initFilePath)
    : mCurrentSceneIdx(currentSceneIdx)
    , mFilePath(initFilePath)
    , mSceneMgr(sceneMgr)
{
    if(!initFilePath.isEmpty()) {
        // TODO: load bounding boxes from file
    }
}

OrientedBoundingBox* BoundingBoxManager::createBox(const Ogre::Vector3& center, const Ogre::Vector3& extents, const Ogre::Matrix3& orientation) {
    OrientedBoundingBox* obb = new OrientedBoundingBox(mSceneMgr, center, extents, orientation);
    mSceneBoxMap[mCurrentSceneIdx].insert(obb);
    return obb;
}

void BoundingBoxManager::destroyBox(OrientedBoundingBox* box) {
    destroyBox(box, mCurrentSceneIdx);
}

void BoundingBoxManager::destroyBox(OrientedBoundingBox* box, unsigned int sceneIdx) {
    if(mSceneBoxMap.contains(sceneIdx))
        mSceneBoxMap[sceneIdx].remove(box);
}

void BoundingBoxManager::onSceneChanging(SceneChangingEventArgs& e) {

}

void BoundingBoxManager::onSceneChanged(SceneChangedEventArgs& e) {

}

void BoundingBoxManager::onMainWindowClosing(WindowClosingEventArgs& e) {

}
