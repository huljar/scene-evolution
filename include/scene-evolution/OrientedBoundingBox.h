#ifndef ORIENTEDBOUNDINGBOX_H
#define ORIENTEDBOUNDINGBOX_H

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>

class OrientedBoundingBox
{
public:
    OrientedBoundingBox(Ogre::SceneManager* sceneMgr, const Ogre::Vector3& center, const Ogre::Vector3& extents, const Ogre::Matrix3& orientation);
    OrientedBoundingBox(const OrientedBoundingBox& other);
    OrientedBoundingBox(OrientedBoundingBox&& other);
    ~OrientedBoundingBox();

    OrientedBoundingBox& operator=(const OrientedBoundingBox& other);
    OrientedBoundingBox& operator=(OrientedBoundingBox&& other);

    Ogre::Vector3 getCenter() const;
    void setCenter(const Ogre::Vector3& center);
    void setCenter(Ogre::Real centerX, Ogre::Real centerY, Ogre::Real centerZ);

    Ogre::Vector3 getExtents() const;
    void setExtents(const Ogre::Vector3& extents);
    void setExtents(Ogre::Real extentX, Ogre::Real extentY, Ogre::Real extentZ);

    Ogre::Quaternion getOrientation() const;
    void setOrientation(const Ogre::Quaternion& orientation);
    void setOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z);

    Ogre::Entity* getEntity() const;
    Ogre::SceneNode* getSceneNode() const;

protected:
    void updateSceneNode();

    Ogre::SceneManager* mSceneMgr;
    Ogre::Entity* mEntity;
    Ogre::SceneNode* mSceneNode;

    Ogre::Vector3 mCenter;
    Ogre::Vector3 mExtents;
    Ogre::Quaternion mOrientation;
};

#endif // ORIENTEDBOUNDINGBOX_H
