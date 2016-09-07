#ifndef BOUNDINGBOXMANAGER_H
#define BOUNDINGBOXMANAGER_H

#include <scene-evolution/OrientedBoundingBox.h>
#include <scene-evolution/EventArgs.h>
#include <scene-evolution/Scene.h>

#include <QObject>
#include <QMap>
#include <QSet>

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreSceneManager.h>

class BoundingBoxManager : public QObject
{
    Q_OBJECT

public:
    typedef QSet<OrientedBoundingBox*> BoxSet;
    typedef QMap<unsigned int, BoxSet> SceneBoxMap;

    BoundingBoxManager(Ogre::SceneManager* sceneMgr, unsigned int currentSceneIdx, const QString& defaultSavePath = QString());
    virtual ~BoundingBoxManager();

    OrientedBoundingBox* createBox(const Ogre::Vector3& center, const Ogre::Vector3& extents,
                                   const Ogre::Quaternion& orientation, const QString& objectType);
    void destroyBox(OrientedBoundingBox* box);
    void destroyBox(OrientedBoundingBox* box, unsigned int sceneIdx);

    bool loadFromFile();
    bool loadFromFile(const QString& path);

    bool saveToFile();
    bool saveToFile(const QString& path);

public slots:
    void onDatasetChanging(DatasetChangingEventArgs& e);
    void onSceneChanging(SceneChangingEventArgs& e);
    void onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

    void onMainWindowClosing(WindowClosingEventArgs& e);

    void onPushButtonStartNewBoxClicked(bool checked);

    void onBoundingBoxCenterValueChanged(double x, double y, double z);
    void onBoundingBoxExtentsValueChanged(double x, double y, double z);
    void onBoundingBoxEulerAnglesValueChanged(double x, double y, double z);

    void onComboBoxBoundingBoxTypeCurrentIndexChanged(const QString& text);

    void onPushButtonFinalizeBoxClicked(bool checked);
    void onPushButtonCancelBoxClicked(bool checked);

private:
    bool checkUnfinalizedBox();
    bool checkUnsavedChanges();

    unsigned int mCurrentSceneIdx;
    OrientedBoundingBox* mCurrentOBB;

    SceneBoxMap mSceneBoxMap;

    QString mFilePath;
    Ogre::SceneManager* mSceneMgr;

    bool mUnsavedChanges;
};

#endif // BOUNDINGBOXMANAGER_H
