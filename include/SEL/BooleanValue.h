#ifndef BOOLEANVALUE_H
#define BOOLEANVALUE_H

#include <SEL/BinaryPredicate.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/Scene.h>

namespace SEL {
    class BooleanValue : public BinaryPredicate
    {
    public:
        BooleanValue(bool value);
        BooleanValue(const BooleanValue& other);
        BooleanValue& operator=(const BooleanValue& other);
        virtual ~BooleanValue();

        virtual bool eval(SceneObjectManager* sceneObjMgr, const Scene& currentScene, SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual BooleanValue* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        bool mValue;
    };
}

#endif // BOOLEANVALUE_H
