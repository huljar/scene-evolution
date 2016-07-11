#include <SEL/NumericValue.h>

using namespace SEL;

NumericValue::NumericValue(QVariant value)
    : mValue(std::move(value))
{
}

NumericValue::~NumericValue() {

}

QVariant NumericValue::calc() const {
    return mValue;
}
