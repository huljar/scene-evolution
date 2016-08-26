#ifndef SCENEOBJECTMANAGER_H
#define SCENEOBJECTMANAGER_H

#include <SEL/SceneObject.h>
#include <scene-evolution/RGBDScene.h>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreQuaternion.h>

#include <opencv2/core/core.hpp>

#include <memory>
#include <vector>

class SceneObjectManager
{
public:
    SceneObjectManager(RGBDScene* rgbdScene, bool showBoundingBoxes = false);
    virtual ~SceneObjectManager();

    bool registerObject(const std::shared_ptr<SEL::SceneObject>& obj);

    void cutObject(const std::shared_ptr<SEL::SceneObject>& obj);
    void cutObjects(const std::vector<std::shared_ptr<SEL::SceneObject>>& objs);

    void updateObjects();

    bool checkObjectInScene(const SEL::SceneObject& obj) const;

    RGBDScene* getRGBDScene() const;

    std::vector<std::shared_ptr<SEL::SceneObject>> getRegisteredObjects() const;

    bool getShowBoundingBoxes() const;
    void setShowBoundingBoxes(bool showBoundingBoxes);

protected:
    RGBDScene* mRGBDScene;

    std::vector<std::shared_ptr<SEL::SceneObject>> mSceneObjects;
    std::vector<Ogre::SceneNode*> mSceneNodes;

    cv::Mat_<unsigned char> mCurrentMask;

    bool mShowBoundingBoxes;

private:
    void addToMask(const cv::Mat_<unsigned char>& mask);
    inline Ogre::Quaternion getQuaternion(const cv::Matx33f rotMat) const;
};

#endif // SCENEOBJECTMANAGER_H
