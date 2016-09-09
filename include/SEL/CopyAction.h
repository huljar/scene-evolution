#ifndef COPYACTION_H
#define COPYACTION_H

#include <SEL/Action.h>
#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

namespace SEL {
    class CopyAction : public Action
    {
    public:
        CopyAction(Object* toObj, SearchCondition* searchCond);
        CopyAction(const CopyAction& other);
        CopyAction& operator=(const CopyAction& other);
        virtual ~CopyAction();

        virtual void exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels,
                          std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const;

        virtual CopyAction* clone() const;

    protected:
        virtual void print(std::ostream &os) const;

        Object* mObj;
        SearchCondition* mSearchCond;
    };
}

#endif // COPYACTION_H
