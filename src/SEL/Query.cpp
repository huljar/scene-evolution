#include <SEL/Query.h>

#include <vector>

using namespace SEL;

Query::Query(SelectStatement* selStmt, std::list<Action*> actList)
    : mSelectStmt(selStmt)
    , mActionList(std::move(actList))
{
}

Query::Query(const Query& other)
    : mSelectStmt(other.mSelectStmt)
    , mActionList(other.mActionList)
{
}

Query& Query::operator=(const Query& other) {
    delete mSelectStmt;
    for(auto it = mActionList.begin(); it != mActionList.end(); ++it)
        delete *it;

    mSelectStmt = other.mSelectStmt;
    mActionList = other.mActionList;

    return *this;
}

Query::~Query() {
    delete mSelectStmt;
    for(auto it = mActionList.begin(); it != mActionList.end(); ++it)
        delete *it;
}

void Query::exec(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const DatasetManager::LabelMap& labels) const {
    std::vector<std::shared_ptr<SceneObject>> objects = mSelectStmt->getSceneObjects(sceneObjMgr, currentScene, labels);

    std::cout << "Objects contains " << objects.size() << " elements:" << std::endl;
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = objects.begin(); it != objects.end(); ++it) {
        std::cout << "  " << (*it)->getName().toStdString() << std::endl;
    }

    std::cout << "Executing actions" << std::endl;

    for(std::list<Action*>::const_iterator it = mActionList.begin(); it != mActionList.end(); ++it) {
        (*it)->exec(sceneObjMgr, currentScene, labels, objects);
    }

    std::cout << "Selected objects contains:" << std::endl;
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = objects.begin(); it != objects.end(); ++it)
        std::cout << "    " << (*it)->getName().toStdString() << ": Translation " << (*it)->getCurrentTranslation() << std::endl;

    // Register selected objects with manager (only non-attached, meshified objects will be registered)
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = objects.begin(); it != objects.end(); ++it)
        sceneObjMgr->registerObject(*it);

    sceneObjMgr->updateObjects();
}

Query* Query::clone() const {
    return new Query(*this);
}

void Query::print(std::ostream& os) const {
    os << "Query containing " << mActionList.size() << " actions";
}

SelectStatement* Query::getSelectStmt() const {
    return mSelectStmt;
}

std::list<Action*> Query::getActionList() const {
    return mActionList;
}
