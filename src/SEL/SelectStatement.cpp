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

QVector<SceneObject> SelectStatement::getSceneObjects(RGBDScene* rgbdScene, const Scene& currentScene, const DatasetManager::LabelMap& labels) const {
    QVector<SceneObject> ret;
    for(std::list<Object*>::const_iterator it = mObjectList.cbegin(); it != mObjectList.cend(); ++it) {
        QVector<SceneObject> current = (*it)->getSceneObjects(*mSearchCond, rgbdScene, currentScene, labels);
        ret.append(current);
    }
    return ret;
}

SelectStatement* SelectStatement::clone() const {
    return new SelectStatement(*this);
}

void SelectStatement::print(std::ostream& os) const {
    os << "SelectStatement with " << mObjectList.size() << " objects";
}
