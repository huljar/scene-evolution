#ifndef BOOLEANVALUE_H
#define BOOLEANVALUE_H

#include <SEL/BinaryPredicate.h>

namespace SEL {
    class BooleanValue : public BinaryPredicate
    {
    public:
        BooleanValue(bool value);
        BooleanValue(const BooleanValue& other);
        BooleanValue& operator=(const BooleanValue& other);
        virtual ~BooleanValue();

        virtual bool eval() const;

        virtual BooleanValue* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        bool mValue;
    };
}

#endif // BOOLEANVALUE_H
