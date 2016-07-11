#include <SEL/Object.h>

using namespace SEL;

Object::Object(std::list<Qualifier*> qualList, QString objName)
    : mQualList(std::move(qualList))
    , mObjName(std::move(objName))
{
}

Object::~Object() {
    for(auto it = mQualList.begin(); it != mQualList.end(); ++it)
        delete *it;
}

QString Object::getObjName() const {
    return mObjName;
}
