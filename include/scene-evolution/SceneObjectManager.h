#ifndef SCENEOBJECTMANAGER_H
#define SCENEOBJECTMANAGER_H

#include <SEL/SceneObject.h>
#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/EventArgs.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/interop.h>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreQuaternion.h>

#include <opencv2/core/core.hpp>

#include <QMap>
#include <QObject>

#include <memory>
#include <tuple>
#include <vector>

class SceneObjectManager : public QObject
{
    Q_OBJECT

public:
    typedef std::shared_ptr<SEL::SceneObject> SceneObjPtr;
    typedef std::vector<std::pair<SceneObjPtr, Ogre::SceneNode*>> ObjVec;
    typedef std::tuple<Scene, RGBDScene*, Ogre::SceneNode*, cv::Mat1b, bool> SceneInfo;

    typedef std::map<unsigned int, ObjVec> SceneObjectsMap;
    typedef std::map<unsigned int, SceneInfo> SceneInfoMap;

    SceneObjectManager(const Scene& currentScene, unsigned int currentSceneIdx, RGBDScene* currentRGBDScene, bool showBoundingBoxes = false);
    virtual ~SceneObjectManager();

    bool registerObject(const SceneObjPtr& obj);
    bool registerObject(const SceneObjPtr& obj, unsigned int sceneIdx);

    void cutObject(const SceneObjPtr& obj);
    void cutObject(const SceneObjPtr& obj, unsigned int sceneIdx);
    void cutObjects(const std::vector<SceneObjPtr>& objs);
    void cutObjects(const std::vector<SceneObjPtr>& objs, unsigned int sceneIdx);

    void updateObjects();

    bool checkObjectInScene(const SEL::SceneObject& obj) const;
    bool checkObjectInScene(const SEL::SceneObject& obj, unsigned int sceneIdx) const;

    RGBDScene* getRGBDScene() const;
    RGBDScene* getRGBDScene(unsigned int sceneIdx) const;
    Ogre::SceneNode* getRGBDSceneNode() const;
    Ogre::SceneNode* getRGBDSceneNode(unsigned int sceneIdx) const;

    ObjVec getRegisteredObjects() const;

    bool getShowBoundingBoxes() const;
    void setShowBoundingBoxes(bool showBoundingBoxes);

public slots:
    void onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

protected:
    unsigned int mCurrentSceneIdx;

    SceneInfoMap mSceneInfoMap;
    SceneObjectsMap mSceneObjectsMap;

    Ogre::SceneManager* mSceneMgr;

    bool mShowBoundingBoxes;

private:
    void addToMask(const cv::Mat1b& mask);
    void addToMask(const cv::Mat1b& mask, unsigned int sceneIdx);

    ObjVec::iterator findObject(const SceneObjPtr& obj);
    ObjVec::iterator findObject(const SceneObjPtr& obj, unsigned int sceneIdx);

    inline Ogre::Quaternion getQuaternion(const cv::Matx33f rotMat) const {
        return Ogre::Quaternion(interop::cvToOgre(rotMat));
    }
};

#endif // SCENEOBJECTMANAGER_H
