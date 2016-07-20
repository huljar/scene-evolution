#ifndef OBJECT_H
#define OBJECT_H

#include <SEL/BisonSymbol.h>
#include <SEL/Qualifier.h>
#include <QString>
#include <list>

namespace SEL {
    class Object : public BisonSymbol
    {
    public:
        Object(std::list<Qualifier*> qualList, QString objName);
        Object(const Object& other);
        Object& operator=(const Object& other);
        virtual ~Object();

        QString getName() const;

        virtual Object* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        std::list<Qualifier*> mQualList;
        QString mObjName;
    };
}

#endif // OBJECT_H
