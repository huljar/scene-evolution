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
}

bool SearchCondition::eval(SceneObjectManager* sceneObjMgr, int sceneIdx, SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    // TODO: make sure that distance term etc. use current locations of objects and not original locations
    if(mLeft)
        return mLeft->eval(sceneObjMgr, sceneIdx, obj, labels) || mRight->eval(sceneObjMgr, sceneIdx, obj, labels);

    return mRight->eval(sceneObjMgr, sceneIdx, obj, labels);
}

SearchCondition* SearchCondition::clone() const {
    return new SearchCondition(*this);
}

void SearchCondition::print(std::ostream& os) const {
    os << "SearchCondition";
}
