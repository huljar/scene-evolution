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

void Query::exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels) const {
    std::vector<std::shared_ptr<SceneObject>> objects = mSelectStmt->getSceneObjects(sceneObjMgr, labels);

    std::cout << "Objects contains " << objects.size() << " elements:" << std::endl;
    for(auto&& obj : objects) {
        std::cout << "  " << obj->getName().toStdString() << std::endl;
    }

    // Cut, meshify and register selected objects with manager
    for(auto&& obj : objects) {
        if(!obj->hasEntity()) {
            Scene scene = sceneObjMgr->getScene(obj->getSceneIdx());
            sceneObjMgr->cutObject(obj);
            obj->meshify(scene.getDepthImg(), scene.getRgbImg(), sceneObjMgr->getRGBDScene(obj->getSceneIdx())->getCameraManager());
            sceneObjMgr->registerObject(obj);
        }
    }

    std::cout << "Executing actions" << std::endl;

    for(auto&& action : mActionList)
        action->exec(sceneObjMgr, labels, objects);

    std::cout << "Selected objects contains: (" << objects.size() << " element" << (objects.size() == 1 ? "" : "s") << ')' << std::endl;
    for(auto&& obj : objects)
        std::cout << "    " << obj->getName().toStdString() << ": Translation " << obj->getCurrentTranslation() << std::endl;

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
