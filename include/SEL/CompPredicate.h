#ifndef COMPPREDICATE_H
#define COMPPREDICATE_H

#include <SEL/Predicate.h>
#include <SEL/CompElement.h>

namespace SEL {
    class CompPredicate : public Predicate
    {
    public:
        enum class Operator {
            EQ,
            NE,
            LT,
            GT,
            LE,
            GE
        };

        CompPredicate(CompElement* left, Operator op, CompElement* right);
        virtual ~CompPredicate();

        virtual bool eval() const;

    protected:
        CompElement* mLeft, *mRight;
        Operator mOp;
    };
}

#endif // COMPPREDICATE_H
