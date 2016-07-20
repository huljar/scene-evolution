#ifndef DISTANCETERM_H
#define DISTANCETERM_H

#include <SEL/CompElement.h>
#include <SEL/Object.h>

namespace SEL {
    class DistanceTerm : public CompElement
    {
    public:
        DistanceTerm(Object* obj);
        DistanceTerm(const DistanceTerm& other);
        DistanceTerm& operator=(const DistanceTerm& other);
        virtual ~DistanceTerm();

        virtual QVariant calc() const;

        virtual DistanceTerm* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Object* mObj;
    };
}

#endif // DISTANCETERM_H
