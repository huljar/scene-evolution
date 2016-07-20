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
        CompPredicate(const CompPredicate& other);
        CompPredicate& operator=(const CompPredicate& other);
        virtual ~CompPredicate();

        virtual bool eval() const;

        virtual CompPredicate* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        CompElement* mLeft, *mRight;
        Operator mOp;
    };
}

#endif // COMPPREDICATE_H
