#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H

#include <SEL/BisonSymbol.h>
#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

#include <scene-evolution/DatasetManager.h>
#include <scene-evolution/SceneObjectManager.h>

#include <QString>
#include <list>
#include <vector>

namespace SEL {
    class SelectStatement : public BisonSymbol
    {
    public:
        SelectStatement(std::list<Object*> objList, int sceneIdx, SearchCondition* searchCond);
        SelectStatement(const SelectStatement& other);
        SelectStatement& operator=(const SelectStatement& other);
        virtual ~SelectStatement();

        virtual std::vector<std::shared_ptr<SceneObject>> getSceneObjects(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels) const;

        virtual SelectStatement* clone() const;

        std::list<Object*> getObjectList() const;
        int getFromSceneIdx() const;
        SearchCondition* getSearchCond() const;

    protected:
        virtual void print(std::ostream& os) const;

        std::list<Object*> mObjectList;
        int mSceneIdx;
        SearchCondition* mSearchCond;
    };
}

#endif // SELECTSTATEMENT_H
