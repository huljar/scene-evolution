#include <SEL/RotateAction.h>
#include <scene-evolution/interop.h>

using namespace SEL;
using namespace interop;

RotateAction::RotateAction(const Ogre::Vector3& angles)
    : mAngles(angles)
{
}

RotateAction::RotateAction(const RotateAction& other)
    : mAngles(other.mAngles)
{
}

RotateAction& RotateAction::operator=(const RotateAction& other) {
    mAngles = other.mAngles;

    return *this;
}

RotateAction::~RotateAction() {

}

void RotateAction::exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels,
                        std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const {
    (void)labels;

    for(auto&& obj : selectedObjects) {
        obj->rotate(ogreToCv(mAngles));
    }
}

RotateAction* RotateAction::clone() const {
    return new RotateAction(*this);
}

void RotateAction::print(std::ostream& os) const {
    os << "RotateAction";
}
