#include <scene-evolution/OrientedBoundingBox.h>
#include <scene-evolution/util.h>

#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreManualObject.h>

#include <iostream>

OrientedBoundingBox::OrientedBoundingBox(Ogre::SceneManager* sceneMgr, const Ogre::Vector3& center, const Ogre::Vector3& extents,
                                         const Ogre::Quaternion& orientation, const QString& objectType)
    : mSceneMgr(sceneMgr)
    , mEntityActive(nullptr)
    , mEntityInactive(nullptr)
    , mSceneNode(nullptr)
    , mCenter(center)
    , mExtents(extents)
    , mOrientation(orientation)
    , mObjectType(objectType)
    , mActive(true)
    , mAttached(true)
{
    // Check if the meshes already exists, if not, create them
    Ogre::MeshPtr meshActive = Ogre::MeshManager::getSingletonPtr()->getByName(Strings::BoundingBoxMeshNameActive);
    if(meshActive.isNull()) {
        std::cout << "Creating the mesh" << std::endl;

        Ogre::ManualObject* m = sceneMgr->createManualObject();
        m->begin(Strings::StandardMaterialName, Ogre::RenderOperation::OT_LINE_LIST);
        createVertices(m, Constants::BoundingBoxActiveColour);
        createIndices(m);
        m->end();
        meshActive = m->convertToMesh(Strings::BoundingBoxMeshNameActive);
    }
    else {
        std::cout << "Mesh already exists" << std::endl;
    }

    Ogre::MeshPtr meshInactive = Ogre::MeshManager::getSingletonPtr()->getByName(Strings::BoundingBoxMeshNameInactive);
    if(meshInactive.isNull()) {
        std::cout << "Creating the mesh" << std::endl;

        Ogre::ManualObject* m = sceneMgr->createManualObject();
        m->begin(Strings::StandardMaterialName, Ogre::RenderOperation::OT_LINE_LIST);
        createVertices(m, Constants::BoundingBoxInactiveColour);
        createIndices(m);
        m->end();
        meshInactive = m->convertToMesh(Strings::BoundingBoxMeshNameInactive);
    }
    else {
        std::cout << "Mesh already exists" << std::endl;
    }

    // Create entities from the meshes for this box
    mEntityActive = mSceneMgr->createEntity(meshActive);
    mEntityInactive = mSceneMgr->createEntity(meshInactive);

    // Create the scene node and attach the active entity
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNode->attachObject(mEntityActive);

    // Apply parameters to scene node
    updateSceneNode();
}

OrientedBoundingBox::OrientedBoundingBox(const OrientedBoundingBox& other)
    : mSceneMgr(other.mSceneMgr)
    , mEntityActive(nullptr)
    , mEntityInactive(nullptr)
    , mSceneNode(nullptr)
    , mCenter(other.mCenter)
    , mExtents(other.mExtents)
    , mOrientation(other.mOrientation)
    , mObjectType(other.mObjectType)
    , mActive(other.mActive)
    , mAttached(other.mAttached)
{
    // Create new entities and scene node
    mEntityActive = mSceneMgr->createEntity(Strings::BoundingBoxMeshNameActive);
    mEntityInactive = mSceneMgr->createEntity(Strings::BoundingBoxMeshNameInactive);
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    if(mAttached)
        mSceneNode->attachObject(mActive ? mEntityActive : mEntityInactive);
    updateSceneNode();
}

OrientedBoundingBox::OrientedBoundingBox(OrientedBoundingBox&& other)
    : mSceneMgr(other.mSceneMgr)
    , mEntityActive(other.mEntityActive)
    , mEntityInactive(other.mEntityInactive)
    , mSceneNode(other.mSceneNode)
    , mCenter(std::move(other.mCenter))
    , mExtents(std::move(other.mExtents))
    , mOrientation(std::move(other.mOrientation))
    , mObjectType(std::move(other.mObjectType))
    , mActive(other.mActive)
    , mAttached(other.mAttached)
{
    // Make other resource-less
    other.mSceneMgr = nullptr;
    other.mEntityActive = nullptr;
    other.mEntityInactive = nullptr;
    other.mSceneNode = nullptr;
}

OrientedBoundingBox::~OrientedBoundingBox() {
    destroy();
}

OrientedBoundingBox& OrientedBoundingBox::operator=(const OrientedBoundingBox& other) {
    destroy();

    mSceneMgr = other.mSceneMgr;
    mCenter = other.mCenter;
    mExtents = other.mExtents;
    mOrientation = other.mOrientation;
    mObjectType = other.mObjectType;
    mActive = other.mActive;
    mAttached = other.mAttached;

    mEntityActive = mSceneMgr->createEntity(Strings::BoundingBoxMeshNameActive);
    mEntityInactive = mSceneMgr->createEntity(Strings::BoundingBoxMeshNameInactive);
    mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    if(mAttached)
        mSceneNode->attachObject(mActive ? mEntityActive : mEntityInactive);
    updateSceneNode();

    return *this;
}

OrientedBoundingBox& OrientedBoundingBox::operator=(OrientedBoundingBox&& other) {
    destroy();

    mSceneMgr = other.mSceneMgr;
    mEntityActive = other.mEntityActive;
    mEntityInactive = other.mEntityInactive;
    mSceneNode = other.mSceneNode;
    mCenter = std::move(other.mCenter);
    mExtents = std::move(other.mExtents);
    mOrientation = std::move(other.mOrientation);
    mObjectType = std::move(other.mObjectType);
    mActive = other.mActive;
    mAttached = other.mAttached;

    other.mSceneMgr = nullptr;
    other.mEntityActive = nullptr;
    other.mEntityInactive = nullptr;
    other.mSceneNode = nullptr;

    return *this;
}

void OrientedBoundingBox::hide() {
    mAttached = false;
    mSceneNode->detachAllObjects();
}

void OrientedBoundingBox::show() {
    mAttached = true;
    mSceneNode->attachObject(mActive ? mEntityActive : mEntityInactive);
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

QString OrientedBoundingBox::getObjectType() const {
    return mObjectType;
}

void OrientedBoundingBox::setObjectType(const QString& objectType) {
    mObjectType = objectType;
}

Ogre::SceneNode* OrientedBoundingBox::getSceneNode() const {
    return mSceneNode;
}

bool OrientedBoundingBox::isActive() const {
    return mActive;
}

void OrientedBoundingBox::setActive(bool active) {
    mActive = active;

    if(mActive) {
        mEntityInactive->detachFromParent();
        mSceneNode->attachObject(mEntityActive);
    }
    else {
        mEntityActive->detachFromParent();
        mSceneNode->attachObject(mEntityInactive);
    }
}

void OrientedBoundingBox::destroy() {
    if(mSceneMgr) {
        mEntityActive->detachFromParent();
        mEntityInactive->detachFromParent();
        mSceneMgr->destroyEntity(mEntityActive);
        mSceneMgr->destroyEntity(mEntityInactive);
        mSceneMgr->destroySceneNode(mSceneNode);
    }
}

void OrientedBoundingBox::createVertices(Ogre::ManualObject* m, const Ogre::ColourValue& c) {
    // Create box corner vertices
    m->position(0.5, 0.5, 0.5); m->colour(c);
    m->position(0.5, 0.5, -0.5); m->colour(c);
    m->position(0.5, -0.5, 0.5); m->colour(c);
    m->position(0.5, -0.5, -0.5); m->colour(c);
    m->position(-0.5, 0.5, 0.5); m->colour(c);
    m->position(-0.5, 0.5, -0.5); m->colour(c);
    m->position(-0.5, -0.5, 0.5); m->colour(c);
    m->position(-0.5, -0.5, -0.5); m->colour(c);
}

void OrientedBoundingBox::createIndices(Ogre::ManualObject* m) {
    // Connect vertices to cube wireframe
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
}

void OrientedBoundingBox::updateSceneNode() {
    mSceneNode->setPosition(mCenter);
    mSceneNode->setOrientation(mOrientation);
    mSceneNode->setScale(mExtents);
}
