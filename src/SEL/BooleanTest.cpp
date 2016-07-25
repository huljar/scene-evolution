#include <SEL/BooleanTest.h>
#include <SEL/SearchCondition.h>

using namespace SEL;

BooleanTest::BooleanTest(Predicate* pred)
    : mPred(pred)
    , mSearchCond(nullptr)
{
}

BooleanTest::BooleanTest(SearchCondition* searchCond)
    : mPred(nullptr)
    , mSearchCond(searchCond)
{
}

BooleanTest::BooleanTest(const BooleanTest& other)
    : mPred(other.mPred)
    , mSearchCond(other.mSearchCond)
{
}

BooleanTest& BooleanTest::operator=(const BooleanTest& other) {
    delete mPred;
    delete mSearchCond;

    mPred = other.mPred;
    mSearchCond = other.mSearchCond;

    return *this;
}

BooleanTest::~BooleanTest() {
    delete mPred;
    delete mSearchCond;
    std::cerr << "Deleting BooleanTest" << std::endl;
}

bool BooleanTest::isPredicate() const {
    return mPred != nullptr;
}

bool BooleanTest::eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const {
    return isPredicate() ? mPred->eval(rgbdScene, currentScene, obj) : mSearchCond->eval(rgbdScene, currentScene, obj);
}

BooleanTest* BooleanTest::clone() const {
    return new BooleanTest(*this);
}

void BooleanTest::print(std::ostream& os) const {
    os << "BooleanTest with " << (isPredicate() ? "predicate" : "nested search condition");
}
