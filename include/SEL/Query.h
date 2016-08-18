#ifndef QUERY_H
#define QUERY_H

#include <SEL/BisonSymbol.h>
#include <SEL/Action.h>
#include <SEL/SelectStatement.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/DatasetManager.h>
#include <scene-evolution/SceneObjectManager.h>

#include <QString>

#include <list>

namespace SEL {
    class Query : public BisonSymbol
    {
    public:
        Query(SelectStatement* selStmt, std::list<Action*> actList);
        Query(const Query& other);
        Query& operator=(const Query& other);
        virtual ~Query();

        virtual void exec(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const DatasetManager::LabelMap& labels) const;

        virtual Query* clone() const;

        SelectStatement* getSelectStmt() const;
        std::list<Action*> getActionList() const;

    protected:
        virtual void print(std::ostream& os) const;

        SelectStatement* mSelectStmt;
        std::list<Action*> mActionList;
    };
}

#endif // QUERY_H
