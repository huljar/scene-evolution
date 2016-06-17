#include <scene-evolution/BoundingBoxManager.h>
#include <scene-evolution/util.h>

#include <OGRE/OgreMatrix3.h>

#include <QMessageBox>

BoundingBoxManager::BoundingBoxManager(Ogre::SceneManager* sceneMgr, unsigned int currentSceneIdx, const QString& initFilePath)
    : mCurrentSceneIdx(currentSceneIdx)
    , mCurrentOBB(nullptr)
    , mFilePath(initFilePath)
    , mSceneMgr(sceneMgr)
{
    if(!initFilePath.isEmpty()) {
        // TODO: load bounding boxes from file
    }
}

OrientedBoundingBox* BoundingBoxManager::createBox(const Ogre::Vector3& center, const Ogre::Vector3& extents, const Ogre::Quaternion& orientation) {
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

void BoundingBoxManager::onDatasetChanging(DatasetChangingEventArgs& e) {

}

void BoundingBoxManager::onDatasetChanged(DatasetChangedEventArgs& e) {

}

void BoundingBoxManager::onSceneChanging(SceneChangingEventArgs& e) {

}

void BoundingBoxManager::onSceneChanged(SceneChangedEventArgs& e) {

}

void BoundingBoxManager::onMainWindowClosing(WindowClosingEventArgs& e) {
    if(false) { // TODO: check if there are unsaved changes
        int ret = QMessageBox::warning(nullptr, tr("Scene Evolution"), tr("There are unsaved changes to the bounding boxes.\nDo you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

        if(ret == QMessageBox::Save) {
            // Save all changes
        }
        else if(ret == QMessageBox::Cancel) {
            // Indicate that the window should not be closed
            e.abort = true;
        }
    }
}

void BoundingBoxManager::onPushButtonStartNewBoxClicked(bool checked) {
    Q_UNUSED(checked);

    //std::cout << "New box" << std::endl;
    if(!mCurrentOBB) {
        Ogre::Matrix3 rot;
        rot.FromEulerAnglesXYZ(Ogre::Degree(Constants::InitialOBBEulerAngles.x), Ogre::Degree(Constants::InitialOBBEulerAngles.y), Ogre::Degree(Constants::InitialOBBEulerAngles.z));
        mCurrentOBB = createBox(Constants::InitialOBBCenter, Constants::InitialOBBExtents, Ogre::Quaternion(rot));
    }
}

void BoundingBoxManager::onBoundingBoxCenterValueChanged(double x, double y, double z) {
    //std::cout << "Center changed: (" << x << ", " << y << ", " << z << ")" << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB->setCenter(x, y, z);
    }
}

void BoundingBoxManager::onBoundingBoxExtentsValueChanged(double x, double y, double z) {
    //std::cout << "Extents changed: (" << x << ", " << y << ", " << z << ")" << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB->setExtents(x, y, z);
    }
}

void BoundingBoxManager::onBoundingBoxEulerAnglesValueChanged(double x, double y, double z) {
    //std::cout << "Euler angles changed: (" << x << ", " << y << ", " << z << ")" << std::endl;
    if(mCurrentOBB) {
        Ogre::Matrix3 rot;
        rot.FromEulerAnglesXYZ(Ogre::Degree(x), Ogre::Degree(y), Ogre::Degree(z));
        mCurrentOBB->setOrientation(Ogre::Quaternion(rot));
    }
}

void BoundingBoxManager::onComboBoxBoundingBoxTypeCurrentIndexChanged(const QString& text) {
    //std::cout << "Type changed: " << text.toStdString() << std::endl;
    if(mCurrentOBB) {
        // TODO: set object type
    }
}

void BoundingBoxManager::onPushButtonFinalizeBoxClicked(bool checked) {
    Q_UNUSED(checked);

    //std::cout << "Finalized box" << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB = nullptr;
    }
}
