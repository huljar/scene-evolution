#ifndef MOVEACTION_H
#define MOVEACTION_H

#include <SEL/Action.h>
#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

namespace SEL {
    class MoveAction : public Action
    {
    public:
        MoveAction(Object* toObj, SearchCondition* searchCond);
        MoveAction(const MoveAction& other);
        MoveAction& operator=(const MoveAction& other);
        virtual ~MoveAction();

        virtual void exec(RGBDScene* rgbdScene, std::vector<SceneObject>& selectedObjects) const;

        virtual MoveAction* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Object* mObj;
        SearchCondition* mSearchCond;
    };
}

#endif // MOVEACTION_H
