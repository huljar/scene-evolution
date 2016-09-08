#include <SEL/RemoveAction.h>

using namespace SEL;

RemoveAction::RemoveAction() {

}

RemoveAction::RemoveAction(const RemoveAction& other) {
    (void)other;
}

RemoveAction& RemoveAction::operator=(const RemoveAction& other) {
    (void)other;
    return *this;
}

RemoveAction::~RemoveAction() {

}

void RemoveAction::exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels,
                        std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const {
    (void)labels;

    for(auto&& obj : selectedObjects) {
        if(!obj->hasManualObject())
            sceneObjMgr->cutObject(obj);

        obj->setVisible(false);
    }
}

RemoveAction* RemoveAction::clone() const {
    return new RemoveAction(*this);
}

void RemoveAction::print(std::ostream& os) const {
    os << "RemoveAction";
}
