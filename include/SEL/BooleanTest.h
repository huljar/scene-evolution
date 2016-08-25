#ifndef BOOLEANTEST_H
#define BOOLEANTEST_H

#include <SEL/BisonSymbol.h>
#include <SEL/Predicate.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/Scene.h>

namespace SEL {
    class SearchCondition; // forward declaration to prevent cycle in inclusion graph

    class BooleanTest : public BisonSymbol
    {
    public:
        BooleanTest(Predicate* pred);
        BooleanTest(SearchCondition* searchCond);
        BooleanTest(const BooleanTest& other);
        BooleanTest& operator=(const BooleanTest& other);
        virtual ~BooleanTest();

        bool isPredicate() const;

        virtual bool eval(SceneObjectManager* rgbdScene, const Scene& currentScene, SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual BooleanTest* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Predicate* mPred;
        SearchCondition* mSearchCond;
    };
}

#endif // BOOLEANTEST_H
