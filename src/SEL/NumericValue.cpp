#include <SEL/NumericValue.h>

using namespace SEL;

NumericValue::NumericValue(QVariant value)
    : mValue(std::move(value))
{
}

NumericValue::NumericValue(const NumericValue& other)
    : mValue(other.mValue)
{
}

NumericValue& NumericValue::operator=(const NumericValue& other) {
    mValue = other.mValue;

    return *this;
}

NumericValue::~NumericValue() {
    std::cerr << "Deleting NumericValue" << std::endl;
}

QVariant NumericValue::calc() const {
    return mValue;
}

NumericValue* NumericValue::clone() const {
    return new NumericValue(*this);
}

void NumericValue::print(std::ostream& os) const {
    os << "NumericValue containing ";
    switch(mValue.type()) {
        case QMetaType::Int: os << "integer " << mValue.toInt(); break;
        case QMetaType::UInt: os << "unsigned integer " << mValue.toUInt(); break;
        case QMetaType::Float: os << "float " << mValue.toFloat(); break;
        case QMetaType::Double: os << "double " << mValue.toDouble(); break;
        default: os << "unexpected value"; break;
    }
}
