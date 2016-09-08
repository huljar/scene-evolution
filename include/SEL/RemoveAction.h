#ifndef REMOVEACTION_H
#define REMOVEACTION_H

#include <SEL/Action.h>

namespace SEL {
    class RemoveAction : public Action
    {
    public:
        RemoveAction();
        RemoveAction(const RemoveAction& other);
        RemoveAction& operator=(const RemoveAction& other);
        virtual ~RemoveAction();

        virtual void exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels,
                          std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const;

        virtual RemoveAction* clone() const;

    protected:
        virtual void print(std::ostream& os) const;
    };
}

#endif // REMOVEACTION_H
