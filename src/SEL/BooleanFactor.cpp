#include <SEL/BooleanFactor.h>

using namespace SEL;

BooleanFactor::BooleanFactor(BooleanTest* child, bool negate)
    : mChild(child)
    , mNegate(negate)
{
}

BooleanFactor::BooleanFactor(const BooleanFactor& other)
    : mChild(other.mChild)
    , mNegate(other.mNegate)
{
}

BooleanFactor& BooleanFactor::operator=(const BooleanFactor& other) {
    delete mChild;

    mChild = other.mChild;
    mNegate = other.mNegate;

    return *this;
}

BooleanFactor::~BooleanFactor() {
    delete mChild;
    std::cerr << "Deleting BooleanFactor" << std::endl;
}

BooleanFactor* BooleanFactor::clone() const {
    return new BooleanFactor(*this);
}

void BooleanFactor::print(std::ostream& os) const {
    os << "BooleanFactor with" << (mNegate ? " " : "out ") << "negation";
}
