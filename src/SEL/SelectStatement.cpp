#include <SEL/SelectStatement.h>

#include <algorithm>
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

std::vector<std::shared_ptr<SceneObject>> SelectStatement::getSceneObjects(SceneObjectManager* sceneObjMgr, const Scene& currentScene,
                                                                           const DatasetManager::LabelMap& labels) const {
    std::vector<std::shared_ptr<SceneObject>> ret;
    for(std::list<Object*>::const_iterator it = mObjectList.cbegin(); it != mObjectList.cend(); ++it) {
        std::vector<std::shared_ptr<SceneObject>> current = (*it)->getSceneObjects(*mSearchCond, sceneObjMgr, currentScene, labels);

        ret.reserve(ret.size() + current.size());
        std::copy(current.begin(), current.end(), std::back_inserter(ret));
    }

    return ret;
}

SelectStatement* SelectStatement::clone() const {
    return new SelectStatement(*this);
}

void SelectStatement::print(std::ostream& os) const {
    os << "SelectStatement with " << mObjectList.size() << " objects";
}
