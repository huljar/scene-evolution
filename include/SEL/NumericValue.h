#ifndef NUMERICVALUE_H
#define NUMERICVALUE_H

#include <SEL/CompElement.h>

namespace SEL {
    class NumericValue : public CompElement
    {
    public:
        NumericValue(QVariant value);
        virtual ~NumericValue();

        virtual QVariant calc() const;

    protected:
        QVariant mValue;
    };
}

#endif // NUMERICVALUE_H
