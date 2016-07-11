#include <SEL/SearchCondition.h>

using namespace SEL;

SearchCondition::SearchCondition(SearchCondition* left, BooleanTerm* right)
    : mLeft(left)
    , mRight(right)
{
}

SearchCondition::~SearchCondition() {
    delete mLeft;
    delete mRight;
}
