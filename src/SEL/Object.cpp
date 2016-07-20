#include <SEL/Object.h>

using namespace SEL;

Object::Object(std::list<Qualifier*> qualList, QString objName)
    : mQualList(std::move(qualList))
    , mObjName(std::move(objName))
{
}

Object::Object(const Object& other)
    : mQualList(other.mQualList)
    , mObjName(other.mObjName)
{
}

Object& Object::operator=(const Object& other) {
    for(auto it = mQualList.begin(); it != mQualList.end(); ++it)
        delete *it;

    mQualList = other.mQualList;
    mObjName = other.mObjName;

    return *this;
}

Object::~Object() {
    for(auto it = mQualList.begin(); it != mQualList.end(); ++it)
        delete *it;
    std::cerr << "Deleting Object" << std::endl;
}

QString Object::getName() const {
    return mObjName;
}

Object* Object::clone() const {
    return new Object(*this);
}

void Object::print(std::ostream& os) const {
    os << "Object with " << mQualList.size() << " qualifiers";
}
