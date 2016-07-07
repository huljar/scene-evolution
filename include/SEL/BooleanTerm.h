#ifndef BOOLEANTERM_H
#define BOOLEANTERM_H

#include <SEL/BooleanFactor.h>

namespace SEL {
    class BooleanTerm
    {
    public:
        BooleanTerm(BooleanTerm* left, BooleanFactor right);

    protected:
        BooleanTerm* mLeft;
        BooleanFactor* mRight;
    };
}

#endif // BOOLEANTERM_H
