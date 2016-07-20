#ifndef ACTION_H
#define ACTION_H

#include <SEL/BisonSymbol.h>

namespace SEL {
    class Action : public BisonSymbol
    {
    public:
        Action();
        virtual ~Action();

        virtual void exec() const = 0;
    };
}

#endif // ACTION_H
