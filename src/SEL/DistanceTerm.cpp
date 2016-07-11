#include <SEL/DistanceTerm.h>

using namespace SEL;

DistanceTerm::DistanceTerm(Object* obj)
    : mObj(obj)
{
}

DistanceTerm::~DistanceTerm() {
    delete mObj;
}

QVariant DistanceTerm::calc() const {
    return QVariant(5); // TODO: implement
}
