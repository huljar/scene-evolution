#ifndef PREDICATE_H
#define PREDICATE_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/DatasetManager.h>

namespace SEL {
    class Predicate : public BisonSymbol
    {
    public:
        Predicate();
        virtual ~Predicate();

        virtual bool eval(SceneObjectManager* sceneObjMgr, int sceneIdx, SceneObject& obj, const DatasetManager::LabelMap& labels) const = 0;
    };
}

#endif // PREDICATE_H
