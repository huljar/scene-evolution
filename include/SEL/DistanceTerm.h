#ifndef DISTANCETERM_H
#define DISTANCETERM_H

#include <SEL/CompElement.h>
#include <SEL/Object.h>

#include <cmath>
#include <random>

namespace SEL {
    class DistanceTerm : public CompElement
    {
    public:
        DistanceTerm(Object* obj);
        DistanceTerm(const DistanceTerm& other);
        DistanceTerm& operator=(const DistanceTerm& other);
        virtual ~DistanceTerm();

        virtual QVariant calc(SceneObjectManager* sceneObjMgr, int sceneIdx, SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual DistanceTerm* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Object* mObj;

        static std::default_random_engine msRandomEngine;
    };
}

#endif // DISTANCETERM_H
