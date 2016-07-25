#ifndef COMPELEMENT_H
#define COMPELEMENT_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>

#include <QVariant>

namespace SEL {
    class CompElement : public BisonSymbol
    {
    public:
        CompElement();
        virtual ~CompElement();

        virtual QVariant calc(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const = 0;
    };
}

#endif // COMPELEMENT_H
