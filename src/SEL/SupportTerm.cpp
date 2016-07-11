#include <SEL/SupportTerm.h>

using namespace SEL;

SupportTerm::SupportTerm(Object* obj)
    : mObj(obj)
{
}

SupportTerm::~SupportTerm() {
    delete mObj;
}

bool SupportTerm::eval() const {
    return true; // TODO: implement
}
