#include <scene-evolution/OrientedBoundingBox.h>
#include <scene-evolution/util.h>

#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreManualObject.h>

#include <iostream>

OrientedBoundingBox::OrientedBoundingBox(Ogre::SceneManager* sceneMgr, const Ogre::Vector3& center, const Ogre::Vector3& extents, const Ogre::Quaternion& orientation)
    : mSceneMgr(sceneMgr)
    , mEntity(nullptr)
    , mSceneNode(nullptr)
    , mCenter(center)
    , mExtents(extents)
    , mOrientation(orientation)
{
    // Check if the mesh already exists, if not, create it
    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingletonPtr()->getByName(Strings::BoundingBoxMeshName);
    if(mesh.isNull()) {
        std::cout << "Creating the mesh" << std::endl;

        Ogre::ManualObject* m = sceneMgr->createManualObject();
        m->begin(Strings::StandardMaterialName, Ogre::RenderOperation::OT_LINE_LIST);

        m->position(0.5, 0.5, 0.5);
        m->position(0.5, 0.5, -0.5);
        m->position(0.5, -0.5, 0.5);
        m->position(0.5, -0.5, -0.5);
        m->position(-0.5, 0.5, 0.5);
        m->position(-0.5, 0.5, -0.5);
        m->position(-0.5, -0.5, 0.5);
        m->position(-0.5, -0.5, -0.5);

        m->index(0); m->index(1);
        m->index(1); m->index(3);
        m->index(3); m->index(2);
        m->index(2); m->index(0);

        m->index(4); m->index(5);
        m->index(5); m->index(7);
        m->index(7); m->index(6);
        m->index(6); m->index(4);

        m->index(0); m->index(4);
        m->index(1); m->index(5);
        m->index(2); m->index(6);
        m->index(3); m->index(7);

        m->end();
        mesh = m->convertToMesh(Strings::BoundingBoxMeshName);
    }
    else {
        std::cout << "Mesh already exists" << std::endl;
    }

    // Create an entity from the mesh for this box
    mEntity = mSceneMgr->createEntity(mesh);

    // Create the scene node and attach the entity
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->attachObject(mEntity);

    // Apply parameters to scene node
    updateSceneNode();
}

OrientedBoundingBox::OrientedBoundingBox(const OrientedBoundingBox& other)
    : mSceneMgr(other.mSceneMgr)
    , mEntity(nullptr)
    , mSceneNode(nullptr)
    , mCenter(other.mCenter)
    , mExtents(other.mExtents)
    , mOrientation(other.mOrientation)
{
    // Create new entity and scene node
    mEntity = mSceneMgr->createEntity(Strings::BoundingBoxMeshName);
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->attachObject(mEntity);
    updateSceneNode();
}

OrientedBoundingBox::OrientedBoundingBox(OrientedBoundingBox&& other)
    : mSceneMgr(other.mSceneMgr)
    , mEntity(other.mEntity)
    , mSceneNode(other.mSceneNode)
    , mCenter(other.mCenter)
    , mExtents(other.mExtents)
    , mOrientation(other.mOrientation)
{
    // Make other resource-less
    other.mSceneMgr = nullptr;
    other.mEntity = nullptr;
    other.mSceneNode = nullptr;
}

OrientedBoundingBox::~OrientedBoundingBox() {
    if(mSceneMgr) {
        mEntity->detachFromParent();
        mSceneMgr->destroyEntity(mEntity);
        mSceneMgr->destroySceneNode(mSceneNode);
    }
}

OrientedBoundingBox& OrientedBoundingBox::operator=(const OrientedBoundingBox& other) {
    if(mSceneMgr) {
        mEntity->detachFromParent();
        mSceneMgr->destroyEntity(mEntity);
        mSceneMgr->destroySceneNode(mSceneNode);
    }

    mSceneMgr = other.mSceneMgr;
    mCenter = other.mCenter;
    mExtents = other.mExtents;
    mOrientation = other.mOrientation;

    mEntity = mSceneMgr->createEntity(Strings::BoundingBoxMeshName);
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->attachObject(mEntity);
    updateSceneNode();

    return *this;
}

OrientedBoundingBox& OrientedBoundingBox::operator=(OrientedBoundingBox&& other) {
    if(mSceneMgr) {
        mEntity->detachFromParent();
        mSceneMgr->destroyEntity(mEntity);
        mSceneMgr->destroySceneNode(mSceneNode);
    }

    mSceneMgr = other.mSceneMgr;
    mEntity = other.mEntity;
    mSceneNode = other.mSceneNode;
    mCenter = other.mCenter;
    mExtents = other.mExtents;
    mOrientation = other.mOrientation;

    other.mSceneMgr = nullptr;
    other.mEntity = nullptr;
    other.mSceneNode = nullptr;

    return *this;
}

Ogre::Vector3 OrientedBoundingBox::getCenter() const {
    return mCenter;
}

void OrientedBoundingBox::setCenter(const Ogre::Vector3& center) {
    mCenter = center;
    updateSceneNode();
}

void OrientedBoundingBox::setCenter(Ogre::Real centerX, Ogre::Real centerY, Ogre::Real centerZ) {
    mCenter = Ogre::Vector3(centerX, centerY, centerZ);
    updateSceneNode();
}

Ogre::Vector3 OrientedBoundingBox::getExtents() const {
    return mExtents;
}

void OrientedBoundingBox::setExtents(const Ogre::Vector3& extents) {
    mExtents = extents;
    updateSceneNode();
}

void OrientedBoundingBox::setExtents(Ogre::Real extentX, Ogre::Real extentY, Ogre::Real extentZ) {
    mExtents = Ogre::Vector3(extentX, extentY, extentZ);
    updateSceneNode();
}

Ogre::Quaternion OrientedBoundingBox::getOrientation() const {
    return mOrientation;
}

void OrientedBoundingBox::setOrientation(const Ogre::Quaternion& orientation) {
    mOrientation = orientation;
    updateSceneNode();
}

void OrientedBoundingBox::setOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z) {
    mOrientation = Ogre::Quaternion(w, x, y, z);
    updateSceneNode();
}

OrientedBoundingBox::ObjectType OrientedBoundingBox::getObjectType() const {
    return mObjectType;
}

void OrientedBoundingBox::setObjectType(ObjectType objectType) {
    mObjectType = objectType;
}

Ogre::Entity* OrientedBoundingBox::getEntity() const {
    return mEntity;
}

Ogre::SceneNode* OrientedBoundingBox::getSceneNode() const {
    return mSceneNode;
}

void OrientedBoundingBox::updateSceneNode() {
    mSceneNode->setPosition(mCenter);
    mSceneNode->setOrientation(mOrientation);
    mSceneNode->setScale(mExtents);
}
