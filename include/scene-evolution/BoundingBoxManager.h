#ifndef BOUNDINGBOXMANAGER_H
#define BOUNDINGBOXMANAGER_H

#include <scene-evolution/OrientedBoundingBox.h>
#include <scene-evolution/EventArgs.h>
#include <scene-evolution/Scene.h>

#include <QObject>
#include <QMap>
#include <QSet>

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreMatrix3.h>
#include <OGRE/OgreSceneManager.h>

class BoundingBoxManager : public QObject
{
    Q_OBJECT

public:
    typedef QSet<OrientedBoundingBox*> BoxSet;
    typedef QMap<unsigned int, BoxSet> SceneBoxMap;

    BoundingBoxManager(Ogre::SceneManager* sceneMgr, unsigned int currentSceneIdx, const QString& initFilePath = QString());

    OrientedBoundingBox* createBox(const Ogre::Vector3& center = Ogre::Vector3(0, 0, -200),
                                   const Ogre::Vector3& extents = Ogre::Vector3(200, 200, 200),
                                   const Ogre::Matrix3& orientation = Ogre::Matrix3::IDENTITY);
    void destroyBox(OrientedBoundingBox* box);
    void destroyBox(OrientedBoundingBox* box, unsigned int sceneIdx);

public slots:
    void onSceneChanging(SceneChangingEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

    void onMainWindowClosing(WindowClosingEventArgs& e);

private:
    unsigned int mCurrentSceneIdx;

    SceneBoxMap mSceneBoxMap;

    QString mFilePath;
    Ogre::SceneManager* mSceneMgr;
};

#endif // BOUNDINGBOXMANAGER_H
