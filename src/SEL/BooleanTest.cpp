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

BooleanTest::~BooleanTest() {
    delete mPred;
    delete mSearchCond;
}

bool BooleanTest::isPredicate() const {
    return mPred != nullptr;
}
