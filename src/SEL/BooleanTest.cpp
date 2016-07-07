#include <SEL/BooleanTest.h>

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

bool BooleanTest::isPredicate() const {
    return mPred != nullptr;
}
