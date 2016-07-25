#ifndef SEARCHCONDITION_H
#define SEARCHCONDITION_H

#include <SEL/BisonSymbol.h>
#include <SEL/BooleanTerm.h>
#include <SEL/Object.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>

namespace SEL {
    class SearchCondition : public BisonSymbol
    {
    public:
        SearchCondition(SearchCondition* left, BooleanTerm* right);
        SearchCondition(const SearchCondition& other);
        SearchCondition& operator=(const SearchCondition& other);
        virtual ~SearchCondition();

        virtual bool eval(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const;

        virtual SearchCondition* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        SearchCondition* mLeft;
        BooleanTerm* mRight;
    };
}

#endif // SEARCHCONDITION_H
