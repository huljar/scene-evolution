#ifndef ACTION_H
#define ACTION_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/DatasetManager.h>

#include <vector>

namespace SEL {
    class Action : public BisonSymbol
    {
    public:
        Action();
        virtual ~Action();

        virtual void exec(RGBDScene* rgbdScene, const Scene& currentScene, const DatasetManager::LabelMap& labels,
                          std::vector<SceneObject>& selectedObjects) const = 0;

    protected:
        virtual void cutObject(SceneObject& obj, RGBDScene* rgbdScene) const;
        virtual void cutObjects(std::vector<SceneObject>& objs, RGBDScene* rgbdScene) const;
    };
}

#endif // ACTION_H
