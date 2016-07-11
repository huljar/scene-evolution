#include <SEL/BooleanTerm.h>

using namespace SEL;

BooleanTerm::BooleanTerm(BooleanTerm* left, BooleanFactor* right)
    : mLeft(left)
    , mRight(right)
{
}

BooleanTerm::~BooleanTerm() {
    delete mLeft;
    delete mRight;
}
