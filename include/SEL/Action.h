#ifndef ACTION_H
#define ACTION_H

#include <iostream>

namespace SEL {
    class Action
    {
    public:
        Action();
        virtual ~Action();

        virtual void exec() const = 0;

        virtual friend std::ostream& operator<<(std::ostream& o, const Action& a) { (void)a; o << "operator<< called on Action"; return o; }
    };
}

#endif // ACTION_H
