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

bool SearchCondition::eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    if(mLeft)
        return mLeft->eval(rgbdScene, currentScene, obj, labels) || mRight->eval(rgbdScene, currentScene, obj, labels);

    return mRight->eval(rgbdScene, currentScene, obj, labels);
}

SearchCondition* SearchCondition::clone() const {
    return new SearchCondition(*this);
}

void SearchCondition::print(std::ostream& os) const {
    os << "SearchCondition";
}
