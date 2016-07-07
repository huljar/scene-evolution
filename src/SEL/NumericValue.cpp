#include <SEL/NumericValue.h>

using namespace SEL;

NumericValue::NumericValue(QVariant value)
    : mValue(std::move(value))
{
}

QVariant NumericValue::calc() const {
    return mValue;
}
