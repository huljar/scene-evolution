#include <scene-evolution/SceneObjectManager.h>

SceneObjectManager::SceneObjectManager(Ogre::SceneManager* sceneMgr)
    : mSceneMgr(sceneMgr)
{

}

bool SceneObjectManager::registerObject(SEL::SceneObject&& obj) {
    // Check if the object has a mesh or if the object is already attached to a scene node
    if(!obj.hasManualObject() || obj.getManualObject()->isAttached()) {
        std::cerr << "Object " << obj.getName().toStdString() << " has no manual object or it is already attached to a scene node." << std::endl;
        return false;
    }

    // Add to vector
    mSceneObjects.push_back(std::move(obj));

    // Add scene node for it
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mSceneNodes.push_back(node);

    // Attach object
    node->attachObject(mSceneObjects.back().getManualObject());

    return true;
}

void SceneObjectManager::updateObjects() {
    for(size_t i = 0; i < mSceneObjects.size(); ++i) {
        cv::Vec3f trans = mSceneObjects[i].getCurrentTranslation();
        cv::Matx33f rot = mSceneObjects[i].getCurrentRotation();

        mSceneNodes[i]->setPosition(Ogre::Vector3(trans[0], trans[1], trans[2]));

        // TODO: rotation
    }
}
