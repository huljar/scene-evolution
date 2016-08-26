#ifndef NUMERICVALUE_H
#define NUMERICVALUE_H

#include <SEL/CompElement.h>

namespace SEL {
    class NumericValue : public CompElement
    {
    public:
        NumericValue(QVariant value);
        NumericValue(const NumericValue& other);
        NumericValue& operator=(const NumericValue& other);
        virtual ~NumericValue();

        virtual QVariant calc(SceneObjectManager* sceneObjMgr, const Scene& currentScene, SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual NumericValue* clone() const;

        QVariant getValue() const;

    protected:
        virtual void print(std::ostream& os) const;

        QVariant mValue;
    };
}

#endif // NUMERICVALUE_H
