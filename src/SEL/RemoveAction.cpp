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
    std::cerr << "Deleting RemoveAction" << std::endl;
}

void RemoveAction::exec(RGBDScene* rgbdScene, std::vector<SceneObject>& selectedObjects) const {
    cutObjects(selectedObjects, rgbdScene);
}

RemoveAction* RemoveAction::clone() const {
    return new RemoveAction(*this);
}

void RemoveAction::print(std::ostream& os) const {
    os << "RemoveAction";
}
