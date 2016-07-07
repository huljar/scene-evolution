#ifndef BOOLEANVALUE_H
#define BOOLEANVALUE_H

#include <SEL/BinaryPredicate.h>

namespace SEL {
    class BooleanValue : public BinaryPredicate
    {
    public:
        BooleanValue(bool value);
        virtual ~BooleanValue();

        virtual bool eval() const;

    protected:
        bool mValue;
    };
}

#endif // BOOLEANVALUE_H
