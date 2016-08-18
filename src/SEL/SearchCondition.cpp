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

bool SearchCondition::eval(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    // TODO: make sure that distance term etc. use current locations of objects and not original locations
    if(mLeft)
        return mLeft->eval(sceneObjMgr, currentScene, obj, labels) || mRight->eval(sceneObjMgr, currentScene, obj, labels);

    return mRight->eval(sceneObjMgr, currentScene, obj, labels);
}

SearchCondition* SearchCondition::clone() const {
    return new SearchCondition(*this);
}

void SearchCondition::print(std::ostream& os) const {
    os << "SearchCondition";
}
