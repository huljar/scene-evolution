#ifndef SCENEOBJECTMANAGER_H
#define SCENEOBJECTMANAGER_H

#include <SEL/SceneObject.h>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <vector>

class SceneObjectManager
{
public:
    SceneObjectManager(Ogre::SceneManager* sceneMgr);

    bool registerObject(SEL::SceneObject&& obj);

    void updateObjects();

protected:
    Ogre::SceneManager* mSceneMgr;

    std::vector<SEL::SceneObject> mSceneObjects;
    std::vector<Ogre::SceneNode*> mSceneNodes;
};

#endif // SCENEOBJECTMANAGER_H
