#include <SEL/BooleanValue.h>

using namespace SEL;

BooleanValue::BooleanValue(bool value)
    : mValue(value)
{
}

BooleanValue::~BooleanValue() {

}

bool BooleanValue::eval() const {
    return mValue;
}
