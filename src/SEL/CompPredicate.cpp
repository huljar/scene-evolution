#include <SEL/CompPredicate.h>

using namespace SEL;

CompPredicate::CompPredicate(CompElement* left, Operator op, CompElement* right)
    : mLeft(left)
    , mRight(right)
    , mOp(op)
{
}

CompPredicate::CompPredicate(const CompPredicate& other)
    : mLeft(other.mLeft)
    , mRight(other.mRight)
    , mOp(other.mOp)
{
}

CompPredicate& CompPredicate::operator=(const CompPredicate& other) {
    delete mLeft;
    delete mRight;

    mLeft = other.mLeft;
    mRight = other.mRight;
    mOp = other.mOp;

    return *this;
}

CompPredicate::~CompPredicate() {
    delete mLeft;
    delete mRight;
    std::cerr << "Deleting CompPredicate" << std::endl;
}

bool CompPredicate::eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    QVariant left = mLeft->calc(rgbdScene, currentScene, obj, labels);
    QVariant right = mRight->calc(rgbdScene, currentScene, obj, labels);

    // Using QMetaType here is correct (see the QVariant documentation at https://doc.qt.io/qt-5/qvariant.html#type )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
    if(left.type() == QMetaType::Int && right.type() == QMetaType::Int) {
        return comp(left.toInt(), right.toInt());
    }
    else if(left.type() == QMetaType::UInt && right.type() == QMetaType::UInt) {
        return comp(left.toUInt(), right.toUInt());
    }
    else if(left.type() == QMetaType::Float && right.type() == QMetaType::Float) {
        return comp(left.toFloat(), right.toFloat());
    }
    else {
        return comp(left.toDouble(), right.toDouble());
    }
#pragma GCC diagnostic pop
}

CompPredicate* CompPredicate::clone() const {
    return new CompPredicate(*this);
}

void CompPredicate::print(std::ostream& os) const {
    os << "CompPredicate containing ";
    switch(mOp) {
        case Operator::EQ: os << "equality"; break;
        case Operator::NE: os << "non-equality"; break;
        case Operator::LT: os << "less-than"; break;
        case Operator::GT: os << "greater-than"; break;
        case Operator::LE: os << "less-or-equal"; break;
        case Operator::GE: os << "greater-or-equal"; break;
        default: os << "unknown"; break;
    }
    os << " comparison";
}
