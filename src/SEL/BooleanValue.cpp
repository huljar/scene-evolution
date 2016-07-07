#include <SEL/BooleanValue.h>

using namespace SEL;

BooleanValue::BooleanValue(bool value)
    : mValue(value)
{
}

bool BooleanValue::eval() const {
    return mValue;
}
