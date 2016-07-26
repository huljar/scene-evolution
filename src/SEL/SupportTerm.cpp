#include <SEL/SupportTerm.h>

using namespace SEL;

SupportTerm::SupportTerm(Object* obj)
    : mObj(obj)
{
}

SupportTerm::SupportTerm(const SupportTerm& other)
    : mObj(other.mObj)
{
}

SupportTerm& SupportTerm::operator=(const SupportTerm& other) {
    delete mObj;

    mObj = other.mObj;

    return *this;
}

SupportTerm::~SupportTerm() {
    delete mObj;
    std::cerr << "Deleting SupportTerm" << std::endl;
}

bool SupportTerm::eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const {
    return true; // TODO: implement
}

SupportTerm* SupportTerm::clone() const {
    return new SupportTerm(*this);
}

void SupportTerm::print(std::ostream& os) const {
    os << "SupportTerm";
}
