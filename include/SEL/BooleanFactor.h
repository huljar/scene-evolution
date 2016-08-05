#ifndef BOOLEANFACTOR_H
#define BOOLEANFACTOR_H

#include <SEL/BisonSymbol.h>
#include <SEL/BooleanTest.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>

namespace SEL {
    class BooleanFactor : public BisonSymbol
    {
    public:
        BooleanFactor(BooleanTest* child, bool negate);
        BooleanFactor(const BooleanFactor& other);
        BooleanFactor& operator=(const BooleanFactor& other);
        virtual ~BooleanFactor();

        virtual bool eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual BooleanFactor* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        BooleanTest* mChild;
        bool mNegate;
    };
}

#endif // BOOLEANFACTOR_H
