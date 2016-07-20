#ifndef BOOLEANFACTOR_H
#define BOOLEANFACTOR_H

#include <SEL/BisonSymbol.h>
#include <SEL/BooleanTest.h>

namespace SEL {
    class BooleanFactor : public BisonSymbol
    {
    public:
        BooleanFactor(BooleanTest* child, bool negate);
        BooleanFactor(const BooleanFactor& other);
        BooleanFactor& operator=(const BooleanFactor& other);
        virtual ~BooleanFactor();

        virtual BooleanFactor* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        BooleanTest* mChild;
        bool mNegate;
    };
}

#endif // BOOLEANFACTOR_H
