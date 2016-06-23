#include <scene-evolution/BoundingBoxManager.h>
#include <scene-evolution/util.h>

#include <OGRE/OgreMatrix3.h>

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <iostream>

BoundingBoxManager::BoundingBoxManager(Ogre::SceneManager* sceneMgr, unsigned int currentSceneIdx, const QString& defaultSavePath)
    : mCurrentSceneIdx(currentSceneIdx)
    , mCurrentOBB(nullptr)
    , mFilePath(defaultSavePath)
    , mSceneMgr(sceneMgr)
    , mUnsavedChanges(false)
{
}

BoundingBoxManager::~BoundingBoxManager() {
    // TODO: destructor (and other constructors?)
    for(SceneBoxMap::iterator it = mSceneBoxMap.begin(); it != mSceneBoxMap.end(); ++it) {
        for(BoxSet::iterator jt = it->begin(); jt != it->end(); ++jt) {
            delete *jt;
        }
    }
}

OrientedBoundingBox* BoundingBoxManager::createBox(const Ogre::Vector3& center, const Ogre::Vector3& extents,
                                                   const Ogre::Quaternion& orientation, const QString& objectType) {
    OrientedBoundingBox* obb = new OrientedBoundingBox(mSceneMgr, center, extents, orientation, objectType);
    mSceneBoxMap[mCurrentSceneIdx].insert(obb);
    return obb;
}

void BoundingBoxManager::destroyBox(OrientedBoundingBox* box) {
    destroyBox(box, mCurrentSceneIdx);
}

void BoundingBoxManager::destroyBox(OrientedBoundingBox* box, unsigned int sceneIdx) {
    if(mSceneBoxMap.contains(sceneIdx))
        mSceneBoxMap[sceneIdx].remove(box);
    delete box;
}

bool BoundingBoxManager::loadFromFile() {
    if(checkUnfinalizedBox())
        return false;
    if(checkUnsavedChanges())
        return false;

    // Get file path
    QString path = QFileDialog::getOpenFileName(nullptr, tr("Load File"), (mFilePath.isEmpty() ? QDir::homePath() : mFilePath),
                                                tr("Oriented Bounding Box Collections (*.obbc)"));

    if(path.isEmpty())
        return false;

    if(!loadFromFile(path)) {
        QMessageBox::warning(nullptr, tr("Scene Evolution"), tr("Unable to load specified file."), QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    mUnsavedChanges = false;
    return true;
}

bool BoundingBoxManager::loadFromFile(const QString& path) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // Save current scene index
    unsigned int oldSceneIdx = mCurrentSceneIdx;

    // Clear map
    for(SceneBoxMap::iterator it = mSceneBoxMap.begin(); it != mSceneBoxMap.end(); ++it) {
        for(BoxSet::iterator jt = it->begin(); jt != it->end(); ++jt) {
            delete *jt;
        }
    }
    mSceneBoxMap.clear();

    // Parse file
    QTextStream in(&file);

    mCurrentSceneIdx = 0;
    OrientedBoundingBox* currentBox = nullptr;

    while(!in.atEnd()) {
        QString line = in.readLine();

        // Discard comments and empty lines
        if(line.length() == 0 || line.startsWith('#'))
            continue;

        // Scene index
        if(line.startsWith("begin scene ") && line.length() > 12)
            mCurrentSceneIdx = line.mid(12).toUInt();

        // New box start
        else if(line.startsWith("begin box")) {
            Ogre::Matrix3 rot;
            rot.FromEulerAnglesXYZ(Ogre::Degree(Constants::InitialOBBEulerAngles.x), Ogre::Degree(Constants::InitialOBBEulerAngles.y), Ogre::Degree(Constants::InitialOBBEulerAngles.z));
            currentBox = createBox(Constants::InitialOBBCenter, Constants::InitialOBBExtents, Ogre::Quaternion(rot), Constants::OBBSupportedObjects.at(Constants::InitialOBBObjectIndex));
            currentBox->setActive(false);
            currentBox->hide();
        }

        // Box center
        else if(line.startsWith("center ")) {
            QTextStream lineStream(&line);
            QString tmp;
            Ogre::Vector3 center;
            lineStream >> tmp >> center.x >> center.y >> center.z;
            currentBox->setCenter(center);
        }

        // Box extents
        else if(line.startsWith("extents ")) {
            QTextStream lineStream(&line);
            QString tmp;
            Ogre::Vector3 extents;
            lineStream >> tmp >> extents.x >> extents.y >> extents.z;
            currentBox->setExtents(extents);
        }

        // Box orientation
        else if(line.startsWith("orient ")) {
            QTextStream lineStream(&line);
            QString tmp;
            Ogre::Quaternion orient;
            lineStream >> tmp >> orient.w >> orient.x >> orient.y >> orient.z;
            currentBox->setOrientation(orient);
        }

        // Box object type
        else if(line.startsWith("objtype "))
            currentBox->setObjectType(line.mid(8));
    }

    // Reset scene index
    mCurrentSceneIdx = oldSceneIdx;

    // Show boxes of current scene
    if(mSceneBoxMap.contains(mCurrentSceneIdx)) {
        BoxSet& boxes = mSceneBoxMap[mCurrentSceneIdx];
        for(BoxSet::iterator it = boxes.begin(); it != boxes.end(); ++it) {
            (*it)->show();
        }
    }

    return true;
}

bool BoundingBoxManager::saveToFile() {
    if(checkUnfinalizedBox())
        return false;

    QString path = QFileDialog::getSaveFileName(nullptr, tr("Save File"), (mFilePath.isEmpty() ? QDir::homePath() : mFilePath),
                                                tr("Oriented Bounding Box Collections (*.obbc)"));

    if(path.isEmpty())
        return false;

    if(!saveToFile(path)) {
        QMessageBox::critical(nullptr, tr("Scene Evolution"), tr("An error occurred while saving the file."), QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    mUnsavedChanges = false;
    return true;
}

bool BoundingBoxManager::saveToFile(const QString& path) {
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);

    // Write header information
    out << "# This is an oriented bounding box collection for the scenes\n"
        << "# of an RGB-D dataset.\n"
        << "# Generated by Scene Evolution tool.\n";

    // Iterate over scenes
    for(SceneBoxMap::iterator it = mSceneBoxMap.begin(); it != mSceneBoxMap.end(); ++it) {
        // Write scene index
        out << "\nbegin scene " << it.key() << "\n";

        // Iterate over boxes
        for(BoxSet::iterator jt = it->begin(); jt != it->end(); ++jt) {
            // Gather box info
            Ogre::Vector3 c = (*jt)->getCenter();
            Ogre::Vector3 e = (*jt)->getExtents();
            Ogre::Quaternion o = (*jt)->getOrientation();
            QString t = (*jt)->getObjectType();

            // Write box info
            out << "\nbegin box\n"
                << "center " << c.x << " " << c.y << " " << c.z << "\n"
                << "extents " << e.x << " " << e.y << " " << e.z << "\n"
                << "orient " << o.w << " " << o.x << " " << o.y << " " << o.z << "\n"
                << "objtype " << t << "\n";
        }
    }

    file.close();

    return true;
}

void BoundingBoxManager::onDatasetChanging(DatasetChangingEventArgs& e) {
    // Check if there is an unfinalized box
    // Doing this in onSceneChanged is not enough because otherwise the check for unsaved changes will be performed before the box check
    if(checkUnfinalizedBox())
        e.abort = true;
    else if(checkUnsavedChanges())
        e.abort = true;
}

void BoundingBoxManager::onDatasetChanged(DatasetChangedEventArgs& e) {
    Q_UNUSED(e);

    // Clear box map
    for(SceneBoxMap::iterator it = mSceneBoxMap.begin(); it != mSceneBoxMap.end(); ++it) {
        for(BoxSet::iterator jt = it->begin(); jt != it->end(); ++jt) {
            delete *jt;
        }
    }
    mSceneBoxMap.clear();
}

void BoundingBoxManager::onSceneChanging(SceneChangingEventArgs& e) {
    e.abort = checkUnfinalizedBox();
}

void BoundingBoxManager::onSceneChanged(SceneChangedEventArgs& e) {
    // Hide boxes of current scene
    if(mSceneBoxMap.contains(mCurrentSceneIdx)) {
        BoxSet& boxes = mSceneBoxMap[mCurrentSceneIdx];
        for(BoxSet::iterator it = boxes.begin(); it != boxes.end(); ++it) {
            (*it)->hide();
        }
    }

    // Change the scene
    mCurrentSceneIdx = e.sceneIdx;

    // Show boxes of new scene (if any already exist)
    if(mSceneBoxMap.contains(e.sceneIdx)) {
        BoxSet& boxes = mSceneBoxMap[e.sceneIdx];
        for(BoxSet::iterator it = boxes.begin(); it != boxes.end(); ++it) {
            (*it)->show();
        }
    }
}

void BoundingBoxManager::onMainWindowClosing(WindowClosingEventArgs& e) {
    if(checkUnfinalizedBox())
        e.abort = true;
    else if(checkUnsavedChanges())
        e.abort = true;
}

void BoundingBoxManager::onPushButtonStartNewBoxClicked(bool checked) {
    Q_UNUSED(checked);

    std::cout << "New box" << std::endl;
    if(!mCurrentOBB) {
        Ogre::Matrix3 rot;
        rot.FromEulerAnglesXYZ(Ogre::Degree(Constants::InitialOBBEulerAngles.x), Ogre::Degree(Constants::InitialOBBEulerAngles.y), Ogre::Degree(Constants::InitialOBBEulerAngles.z));
        mCurrentOBB = createBox(Constants::InitialOBBCenter, Constants::InitialOBBExtents, Ogre::Quaternion(rot), Constants::OBBSupportedObjects.at(Constants::InitialOBBObjectIndex));
    }
}

void BoundingBoxManager::onBoundingBoxCenterValueChanged(double x, double y, double z) {
    std::cout << "Center changed: (" << x << ", " << y << ", " << z << ")" << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB->setCenter(x, y, z);
    }
}

void BoundingBoxManager::onBoundingBoxExtentsValueChanged(double x, double y, double z) {
    std::cout << "Extents changed: (" << x << ", " << y << ", " << z << ")" << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB->setExtents(x, y, z);
    }
}

void BoundingBoxManager::onBoundingBoxEulerAnglesValueChanged(double x, double y, double z) {
    std::cout << "Euler angles changed: (" << x << ", " << y << ", " << z << ")" << std::endl;
    if(mCurrentOBB) {
        Ogre::Matrix3 rot;
        rot.FromEulerAnglesXYZ(Ogre::Degree(x), Ogre::Degree(y), Ogre::Degree(z));
        mCurrentOBB->setOrientation(Ogre::Quaternion(rot));
    }
}

void BoundingBoxManager::onComboBoxBoundingBoxTypeCurrentIndexChanged(const QString& text) {
    std::cout << "Type changed: " << text.toStdString() << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB->setObjectType(text);
    }
}

void BoundingBoxManager::onPushButtonFinalizeBoxClicked(bool checked) {
    Q_UNUSED(checked);

    std::cout << "Finalized box" << std::endl;
    if(mCurrentOBB) {
        mCurrentOBB->setActive(false);
        mCurrentOBB = nullptr;
    }

    mUnsavedChanges = true;
}

void BoundingBoxManager::onPushButtonCancelBoxClicked(bool checked) {
    Q_UNUSED(checked);

    std::cout << "Canceled box" << std::endl;
    if(mCurrentOBB) {
        destroyBox(mCurrentOBB);
        mCurrentOBB = nullptr;
    }
}

bool BoundingBoxManager::checkUnfinalizedBox() {
    // Check if there is an unfinalized box
    if(mCurrentOBB) {
        int ret = QMessageBox::warning(nullptr, tr("Scene Evolution"),
                                       tr("You have an unfinalized bounding box for the current scene.\nDo you want to finalize it?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

        if(ret == QMessageBox::Yes) {
            onPushButtonFinalizeBoxClicked(false);
        }
        else if(ret == QMessageBox::No) {
            onPushButtonCancelBoxClicked(false);
        }
        else { // ret == Cancel
            return true;
        }
    }

    return false;
}

bool BoundingBoxManager::checkUnsavedChanges() {
    // Check for unsaved changes
    if(mUnsavedChanges) {
        int ret = QMessageBox::warning(nullptr, tr("Scene Evolution"), tr("There are unsaved changes to the bounding boxes.\nDo you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

        if(ret == QMessageBox::Save) {
            // Save all changes
            QString path = QFileDialog::getSaveFileName(nullptr, tr("Save File"), (mFilePath.isEmpty() ? QDir::homePath() : mFilePath),
                                                        tr("Oriented Bounding Box Collections (*.obbc)"));
            if(path.isEmpty()) {
                // If the file dialog is canceled, abort the window closing
                return true;
            }
            else {
                mFilePath = path;
                bool success = saveToFile(path);
                if(success) {
                    mUnsavedChanges = false;
                }
                else {
                    QMessageBox::critical(nullptr, tr("Scene Evolution"), tr("An error occurred while saving the file."), QMessageBox::Ok, QMessageBox::Ok);
                    return true;
                }
            }
        }
        else if(ret == QMessageBox::Discard) {
            mUnsavedChanges = false;
        }
        else { // ret == Cancel
            // Indicate that the window should not be closed
            return true;
        }
    }

    return false;
}
