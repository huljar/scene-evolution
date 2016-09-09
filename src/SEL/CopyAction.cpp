#include <SEL/CopyAction.h>

using namespace SEL;

CopyAction::CopyAction(Object* toObj, SearchCondition* searchCond)
    : mObj(toObj)
    , mSearchCond(searchCond)
{
}

CopyAction::CopyAction(const CopyAction& other)
    : mObj(other.mObj)
    , mSearchCond(other.mSearchCond)
{
}

CopyAction& CopyAction::operator=(const CopyAction& other) {
    delete mObj;
    delete mSearchCond;

    mObj = other.mObj;
    mSearchCond = other.mSearchCond;

    return *this;
}

CopyAction::~CopyAction() {
    delete mObj;
    delete mSearchCond;
}

void CopyAction::exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels, std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const {

}

CopyAction* CopyAction::clone() const {
    return new CopyAction(*this);
}

void CopyAction::print(std::ostream& os) const {
    os << "CopyAction";
}
