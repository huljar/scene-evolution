#ifndef OBJECT_H
#define OBJECT_H

#include <SEL/Qualifier.h>
#include <QString>
#include <list>

namespace SEL {
    class Object
    {
    public:
        Object(std::list<Qualifier*> qualList, QString objName);
        virtual ~Object();

        QString getObjName() const;

        friend std::ostream& operator<<(std::ostream& o, const Object& obj) { o << "operator<< called on Object " << obj.mObjName.toStdString(); return o; }

    protected:
        std::list<Qualifier*> mQualList;
        QString mObjName;
    };
}

#endif // OBJECT_H
