#ifndef REMOVEACTION_H
#define REMOVEACTION_H

#include <SEL/Action.h>

namespace SEL {
    class RemoveAction : public Action
    {
    public:
        RemoveAction();
        virtual ~RemoveAction();

        virtual void exec() const;
    };
}

#endif // REMOVEACTION_H
