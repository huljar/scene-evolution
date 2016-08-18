#ifndef MOVEACTION_H
#define MOVEACTION_H

#include <SEL/Action.h>
#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

#include <random>

namespace SEL {
    class MoveAction : public Action
    {
    public:
        MoveAction(Object* toObj, SearchCondition* searchCond);
        MoveAction(const MoveAction& other);
        MoveAction& operator=(const MoveAction& other);
        virtual ~MoveAction();

        virtual void exec(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const DatasetManager::LabelMap& labels,
                          std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const;

        virtual MoveAction* clone() const;

    protected:
        float slope(const cv::Vec3f& a, const cv::Vec3f& b) const;

        virtual void print(std::ostream& os) const;

        Object* mObj;
        SearchCondition* mSearchCond;

        static std::default_random_engine msRandomEngine;
    };
}

#endif // MOVEACTION_H
