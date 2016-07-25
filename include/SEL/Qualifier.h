#ifndef QUALIFIER_H
#define QUALIFIER_H

#include <SEL/BisonSymbol.h>
#include <QString>

namespace SEL {
    class Qualifier : public BisonSymbol
    {
    public:
        Qualifier(QString text);
        Qualifier(const Qualifier& other);
        Qualifier& operator=(const Qualifier& other);
        virtual ~Qualifier();

        virtual Qualifier* clone() const;

        QString getText() const;

    protected:
        virtual void print(std::ostream& os) const;

        QString mText;
    };
}

#endif // QUALIFIER_H
