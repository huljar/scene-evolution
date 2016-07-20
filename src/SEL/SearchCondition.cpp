#include <SEL/SearchCondition.h>

using namespace SEL;

SearchCondition::SearchCondition(SearchCondition* left, BooleanTerm* right)
    : mLeft(left)
    , mRight(right)
{
}

SearchCondition::SearchCondition(const SearchCondition& other)
    : mLeft(other.mLeft)
    , mRight(other.mRight)
{
}

SearchCondition& SearchCondition::operator=(const SearchCondition& other) {
    delete mLeft;
    delete mRight;

    mLeft = other.mLeft;
    mRight = other.mRight;

    return *this;
}

SearchCondition::~SearchCondition() {
    delete mLeft;
    delete mRight;
    std::cerr << "Deleting SearchCondition" << std::endl;
}

SearchCondition* SearchCondition::clone() const {
    return new SearchCondition(*this);
}

void SearchCondition::print(std::ostream& os) const {
    os << "SearchCondition";
}
