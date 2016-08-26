#ifndef ROTATEACTION_H
#define ROTATEACTION_H

#include <SEL/Action.h>

#include <OGRE/OgreVector3.h>

namespace SEL {
    class RotateAction : public Action
    {
    public:
        RotateAction(const Ogre::Vector3& angles);
        RotateAction(const RotateAction& other);
        RotateAction& operator=(const RotateAction& other);
        virtual ~RotateAction();

        virtual void exec(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const DatasetManager::LabelMap& labels,
                          std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const;

        virtual RotateAction* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Ogre::Vector3 mAngles;
    };
}

#endif // ROTATEACTION_H
