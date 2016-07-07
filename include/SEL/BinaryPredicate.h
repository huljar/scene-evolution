#ifndef BINARYPREDICATE_H
#define BINARYPREDICATE_H

#include <SEL/Predicate.h>

namespace SEL {
    class BinaryPredicate : public Predicate
    {
    public:
        BinaryPredicate();
        virtual ~BinaryPredicate();

        virtual bool eval() const = 0;
    };
}

#endif // BINARYPREDICATE_H
