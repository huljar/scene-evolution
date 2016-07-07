#ifndef SUPPORTTERM_H
#define SUPPORTTERM_H

#include <SEL/BinaryPredicate.h>
#include <SEL/Object.h>

namespace SEL {
    class SupportTerm : public BinaryPredicate
    {
    public:
        SupportTerm(Object* obj);
        virtual ~SupportTerm();

        virtual bool eval() const;

    protected:
        Object* mObj;
    };
}

#endif // SUPPORTTERM_H
