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

        virtual void exec() const;

        virtual RemoveAction* clone() const;

    protected:
        virtual void print(std::ostream& os) const;
    };
}

#endif // REMOVEACTION_H
