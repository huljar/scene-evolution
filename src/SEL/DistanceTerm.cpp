#include <SEL/DistanceTerm.h>

using namespace SEL;

DistanceTerm::DistanceTerm(Object* obj)
    : mObj(obj)
{
}

DistanceTerm::DistanceTerm(const DistanceTerm& other)
    : mObj(other.mObj)
{
}

DistanceTerm& DistanceTerm::operator=(const DistanceTerm& other) {
    delete mObj;

    mObj = other.mObj;

    return *this;
}

DistanceTerm::~DistanceTerm() {
    delete mObj;
    std::cerr << "Deleting DistanceTerm" << std::endl;
}

QVariant DistanceTerm::calc() const {
    return QVariant(5); // TODO: implement
}

DistanceTerm* DistanceTerm::clone() const {
    return new DistanceTerm(*this);
}

void DistanceTerm::print(std::ostream& os) const {
    os << "DistanceTerm";
}
