#ifndef PREDICATE_H
#define PREDICATE_H

#include <SEL/BisonSymbol.h>

namespace SEL {
    class Predicate : public BisonSymbol
    {
    public:
        Predicate();
        virtual ~Predicate();

        virtual bool eval() const = 0;
    };
}

#endif // PREDICATE_H
