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

    BoundingBoxManager(Ogre::SceneManager* sceneMgr, unsigned int currentSceneIdx, const QString& initFilePath = QString());
    ~BoundingBoxManager();

    OrientedBoundingBox* createBox(const Ogre::Vector3& center,
                                   const Ogre::Vector3& extents,
                                   const Ogre::Quaternion& orientation);
    void destroyBox(OrientedBoundingBox* box);
    void destroyBox(OrientedBoundingBox* box, unsigned int sceneIdx);

public slots:
    void onDatasetChanging(DatasetChangingEventArgs& e);
    void onDatasetChanged(DatasetChangedEventArgs& e);

    void onSceneChanging(SceneChangingEventArgs& e);
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
    unsigned int mCurrentSceneIdx;
    OrientedBoundingBox* mCurrentOBB;

    SceneBoxMap mSceneBoxMap;

    QString mFilePath;
    Ogre::SceneManager* mSceneMgr;
};

#endif // BOUNDINGBOXMANAGER_H
