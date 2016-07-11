#ifndef BOOLEANFACTOR_H
#define BOOLEANFACTOR_H

#include <SEL/BooleanTest.h>

namespace SEL {
    class BooleanFactor
    {
    public:
        BooleanFactor(BooleanTest* child, bool negate);
        virtual ~BooleanFactor();

    protected:
        BooleanTest* mChild;
        bool mNegate;
    };
}

#endif // BOOLEANFACTOR_H
