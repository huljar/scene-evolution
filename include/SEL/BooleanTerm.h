#ifndef BOOLEANTERM_H
#define BOOLEANTERM_H

#include <SEL/BisonSymbol.h>
#include <SEL/BooleanFactor.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>

namespace SEL {
    class BooleanTerm : public BisonSymbol
    {
    public:
        BooleanTerm(BooleanTerm* left, BooleanFactor* right);
        BooleanTerm(const BooleanTerm& other);
        BooleanTerm& operator=(const BooleanTerm& other);
        virtual ~BooleanTerm();

        virtual bool eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const;

        virtual BooleanTerm* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        BooleanTerm* mLeft;
        BooleanFactor* mRight;
    };
}

#endif // BOOLEANTERM_H
