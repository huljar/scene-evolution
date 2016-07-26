#ifndef PREDICATE_H
#define PREDICATE_H

#include <SEL/BisonSymbol.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>

namespace SEL {
    class Predicate : public BisonSymbol
    {
    public:
        Predicate();
        virtual ~Predicate();

        virtual bool eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const = 0;
    };
}

#endif // PREDICATE_H
