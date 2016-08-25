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

}

QVariant NumericValue::calc(SceneObjectManager* sceneObjMgr, const Scene& currentScene, SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    (void)sceneObjMgr;
    (void)currentScene;
    (void)obj;
    (void)labels;

    return mValue;
}

NumericValue* NumericValue::clone() const {
    return new NumericValue(*this);
}

void NumericValue::print(std::ostream& os) const {
    os << "NumericValue containing ";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch(mValue.type()) { // Use of QMetaType is correct here (see the QVariant documentation at https://doc.qt.io/qt-5/qvariant.html#type )
        case QMetaType::Int: os << "integer " << mValue.toInt(); break;
        case QMetaType::UInt: os << "unsigned integer " << mValue.toUInt(); break;
        case QMetaType::Float: os << "float " << mValue.toFloat(); break;
        case QMetaType::Double: os << "double " << mValue.toDouble(); break;
        default: os << "unexpected value"; break;
    }
#pragma GCC diagnostic pop
}
