#ifndef MOVEACTION_H
#define MOVEACTION_H

#include <SEL/Action.h>
#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

namespace SEL {
    class MoveAction : public Action
    {
    public:
        MoveAction(Object* obj, SearchCondition* searchCond);
        virtual ~MoveAction();

        virtual void exec() const;

    protected:
        Object* mObj;
        SearchCondition* mSearchCond;
    };
}

#endif // MOVEACTION_H
