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

    protected:
        std::list<Qualifier*> mQualList;
        QString mObjName;
    };
}

#endif // OBJECT_H
