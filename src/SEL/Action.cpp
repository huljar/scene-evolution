#include <SEL/Action.h>

using namespace SEL;

Action::Action() {

}

Action::~Action() {

}

void Action::cutObject(SceneObject& obj, RGBDScene* rgbdScene) const {

}

void Action::cutObjects(QVector<SceneObject>& objs, RGBDScene* rgbdScene) const {
    for(QVector<SceneObject>::iterator it = objs.begin(); it != objs.end(); ++it)
        cutObject(*it, rgbdScene);
}
