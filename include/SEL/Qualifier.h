#ifndef QUALIFIER_H
#define QUALIFIER_H

#include <QString>

namespace SEL {
    class Qualifier
    {
    public:
        Qualifier(QString text);
        virtual ~Qualifier();

    protected:
        QString mText;
    };
}

#endif // QUALIFIER_H
