#ifndef COMPELEMENT_H
#define COMPELEMENT_H

#include <QVariant>

namespace SEL {
    class CompElement
    {
    public:
        CompElement();
        virtual ~CompElement();

        virtual QVariant calc() const = 0;
    };
}

#endif // COMPELEMENT_H
