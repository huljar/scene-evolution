#ifndef ACTION_H
#define ACTION_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/DatasetManager.h>
#include <scene-evolution/SceneObjectManager.h>

#include <memory>
#include <vector>

namespace SEL {
    class Action : public BisonSymbol
    {
    public:
        Action();
        virtual ~Action();

        virtual void exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels,
                          std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const = 0;
    };
}

#endif // ACTION_H
