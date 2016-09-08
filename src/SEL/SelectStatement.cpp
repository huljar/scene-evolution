#include <SEL/SelectStatement.h>

#include <algorithm>
#include <queue>

using namespace SEL;

SelectStatement::SelectStatement(std::list<Object*> objList, int sceneIdx, SearchCondition* searchCond)
    : mObjectList(std::move(objList))
    , mSceneIdx(sceneIdx)
    , mSearchCond(searchCond)
{
}

SelectStatement::SelectStatement(const SelectStatement& other)
    : mObjectList(other.mObjectList)
    , mSceneIdx(other.mSceneIdx)
    , mSearchCond(other.mSearchCond)
{
}

SelectStatement& SelectStatement::operator=(const SelectStatement& other) {
    for(auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
        delete *it;
    delete mSearchCond;

    mObjectList = other.mObjectList;
    mSceneIdx = other.mSceneIdx;
    mSearchCond = other.mSearchCond;

    return *this;
}

SelectStatement::~SelectStatement() {
    for(auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
        delete *it;
    delete mSearchCond;
}

std::vector<std::shared_ptr<SceneObject>> SelectStatement::getSceneObjects(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels) const {
    std::vector<std::shared_ptr<SceneObject>> ret;
    for(auto&& objPtr : mObjectList) {
        std::vector<std::shared_ptr<SceneObject>> current = (mSearchCond
                                                             ? objPtr->getSceneObjects(*mSearchCond, sceneObjMgr, mSceneIdx, labels)
                                                             : objPtr->getSceneObjects(sceneObjMgr, mSceneIdx, labels));

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

std::list<Object*> SelectStatement::getObjectList() const {
    return mObjectList;
}

int SelectStatement::getFromSceneIdx() const {
    return mSceneIdx;
}

SearchCondition* SelectStatement::getSearchCond() const {
    return mSearchCond;
}
