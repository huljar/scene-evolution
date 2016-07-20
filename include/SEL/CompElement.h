#ifndef COMPELEMENT_H
#define COMPELEMENT_H

#include <SEL/BisonSymbol.h>
#include <QVariant>

namespace SEL {
    class CompElement : public BisonSymbol
    {
    public:
        CompElement();
        virtual ~CompElement();

        virtual QVariant calc() const = 0;
    };
}

#endif // COMPELEMENT_H
