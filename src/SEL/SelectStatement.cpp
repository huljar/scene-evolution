#include <SEL/SelectStatement.h>

#include <queue>

using namespace SEL;

SelectStatement::SelectStatement(std::list<Object*> objList, SearchCondition* searchCond)
    : mObjectList(std::move(objList))
    , mSearchCond(searchCond)
{
}

SelectStatement::SelectStatement(const SelectStatement& other)
    : mObjectList(other.mObjectList)
    , mSearchCond(other.mSearchCond)
{
}

SelectStatement& SelectStatement::operator=(const SelectStatement& other) {
    for(auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
        delete *it;
    delete mSearchCond;

    mObjectList = other.mObjectList;
    mSearchCond = other.mSearchCond;

    return *this;
}

SelectStatement::~SelectStatement() {
    for(auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
        delete *it;
    delete mSearchCond;
    std::cerr << "Deleting SelectStatement" << std::endl;
}

std::vector<SceneObject> SelectStatement::getSceneObjects(RGBDScene* rgbdScene, const Scene& currentScene, const DatasetManager::LabelMap& labels) const {
    std::vector<SceneObject> ret;
    for(std::list<Object*>::const_iterator it = mObjectList.cbegin(); it != mObjectList.cend(); ++it) {
        std::vector<SceneObject> current = (*it)->getSceneObjects(*mSearchCond, rgbdScene, currentScene, labels);

        ret.reserve(ret.size() + current.size());
        for(std::vector<SceneObject>::iterator it = current.begin(); it != current.end(); ++it) {
            ret.push_back(std::move(*it));
        }
    }
    return ret;
}

SelectStatement* SelectStatement::clone() const {
    return new SelectStatement(*this);
}

void SelectStatement::print(std::ostream& os) const {
    os << "SelectStatement with " << mObjectList.size() << " objects";
}
