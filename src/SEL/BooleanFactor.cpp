#include <SEL/BooleanFactor.h>

using namespace SEL;

BooleanFactor::BooleanFactor(BooleanTest* child, bool negate)
    : mChild(child)
    , mNegate(negate)
{
}
