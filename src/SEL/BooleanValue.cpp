#include <SEL/BooleanValue.h>
#include <iomanip>

using namespace SEL;

BooleanValue::BooleanValue(bool value)
    : mValue(value)
{
}

BooleanValue::BooleanValue(const BooleanValue& other)
    : mValue(other.mValue)
{
}

BooleanValue& BooleanValue::operator=(const BooleanValue& other) {
    mValue = other.mValue;

    return *this;
}

BooleanValue::~BooleanValue() {
    std::cerr << "Deleting BooleanValue" << std::endl;
}

bool BooleanValue::eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const {
    (void)rgbdScene;
    (void)currentScene;
    (void)obj;

    return mValue;
}

BooleanValue* BooleanValue::clone() const {
    return new BooleanValue(*this);
}

void BooleanValue::print(std::ostream& os) const {
    os << "BooleanValue containing " << std::boolalpha << mValue << std::noboolalpha;
}
