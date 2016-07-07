#ifndef DISTANCETERM_H
#define DISTANCETERM_H

#include <SEL/CompElement.h>
#include <SEL/Object.h>

namespace SEL {
    class DistanceTerm : public CompElement
    {
    public:
        DistanceTerm(Object* obj);
        virtual ~DistanceTerm();

        virtual QVariant calc() const;

    protected:
        Object* mObj;
    };
}

#endif // DISTANCETERM_H
