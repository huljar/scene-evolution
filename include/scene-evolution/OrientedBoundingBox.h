#ifndef ORIENTEDBOUNDINGBOX_H
#define ORIENTEDBOUNDINGBOX_H

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>

#include <QString>

class OrientedBoundingBox
{
public:
    // Constructors and destructor
    OrientedBoundingBox(Ogre::SceneManager* sceneMgr, const Ogre::Vector3& center, const Ogre::Vector3& extents,
                        const Ogre::Quaternion& orientation, const QString& objectType);
    OrientedBoundingBox(const OrientedBoundingBox& other);
    OrientedBoundingBox(OrientedBoundingBox&& other);
    ~OrientedBoundingBox();

    // Assignment operators
    OrientedBoundingBox& operator=(const OrientedBoundingBox& other);
    OrientedBoundingBox& operator=(OrientedBoundingBox&& other);

    // Visibility
    void hide();
    void show();

    // Getters and setters
    Ogre::Vector3 getCenter() const;
    void setCenter(const Ogre::Vector3& center);
    void setCenter(Ogre::Real centerX, Ogre::Real centerY, Ogre::Real centerZ);

    Ogre::Vector3 getExtents() const;
    void setExtents(const Ogre::Vector3& extents);
    void setExtents(Ogre::Real extentX, Ogre::Real extentY, Ogre::Real extentZ);

    Ogre::Quaternion getOrientation() const;
    void setOrientation(const Ogre::Quaternion& orientation);
    void setOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z);

    QString getObjectType() const;
    void setObjectType(const QString& objectType);

    Ogre::SceneNode* getSceneNode() const;

    bool isActive() const;
    void setActive(bool active);

protected:
    void destroy();

    void createVertices(Ogre::ManualObject* m, const Ogre::ColourValue& c = Ogre::ColourValue::White);
    void createIndices(Ogre::ManualObject* m);
    void updateSceneNode();

    Ogre::SceneManager* mSceneMgr;
    Ogre::Entity* mEntityActive;
    Ogre::Entity* mEntityInactive;
    Ogre::SceneNode* mSceneNode;

    Ogre::Vector3 mCenter;
    Ogre::Vector3 mExtents;
    Ogre::Quaternion mOrientation;

    QString mObjectType;

    bool mActive;
    bool mAttached;
};

#endif // ORIENTEDBOUNDINGBOX_H
