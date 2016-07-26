#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H

#include <SEL/BisonSymbol.h>
#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

#include <scene-evolution/DatasetManager.h>

#include <QString>
#include <list>

namespace SEL {
    class SelectStatement : public BisonSymbol
    {
    public:
        SelectStatement(std::list<Object*> objList, SearchCondition* searchCond);
        SelectStatement(const SelectStatement& other);
        SelectStatement& operator=(const SelectStatement& other);
        virtual ~SelectStatement();

        virtual QVector<SceneObject> getSceneObjects(RGBDScene* rgbdScene, const Scene& currentScene, const DatasetManager::LabelMap& labels) const;

        virtual SelectStatement* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        std::list<Object*> mObjectList;
        SearchCondition* mSearchCond;
    };
}

#endif // SELECTSTATEMENT_H
