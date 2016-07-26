#include <SEL/Query.h>

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
    std::cerr << "Deleting Query" << std::endl;
}

void Query::exec(RGBDScene* rgbdScene, const Scene& currentScene, const DatasetManager::LabelMap& labels) const {
    QVector<SceneObject> objects = mSelectStmt->getSceneObjects(rgbdScene, currentScene, labels);
    std::cout << "Objects contains " << objects.size() << " elements:" << std::endl;
    for(QVector<SceneObject>::iterator it = objects.begin(); it != objects.end(); ++it) {
        std::cout << "  " << it->getName().toStdString() << std::endl;
    }
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
