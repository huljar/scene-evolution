#ifndef COMPELEMENT_H
#define COMPELEMENT_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/DatasetManager.h>

#include <QVariant>

namespace SEL {
    class CompElement : public BisonSymbol
    {
    public:
        CompElement();
        virtual ~CompElement();

        virtual QVariant calc(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const = 0;
    };
}

#endif // COMPELEMENT_H
