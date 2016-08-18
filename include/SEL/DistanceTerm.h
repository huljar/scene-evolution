#ifndef DISTANCETERM_H
#define DISTANCETERM_H

#include <SEL/CompElement.h>
#include <SEL/Object.h>

#include <cmath>

namespace SEL {
    class DistanceTerm : public CompElement
    {
    public:
        DistanceTerm(Object* obj);
        DistanceTerm(const DistanceTerm& other);
        DistanceTerm& operator=(const DistanceTerm& other);
        virtual ~DistanceTerm();

        virtual QVariant calc(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const;

        virtual DistanceTerm* clone() const;

    protected:
        virtual void print(std::ostream& os) const;

        Object* mObj;

    private:
        inline float dist(const cv::Vec3f& a, const cv::Vec3f& b) const {
            return std::sqrt(std::pow(a[0] - b[0], 2) + std::pow(a[1] - b[1], 2) + std::pow(a[2] - b[2], 2));
        }

    };
}

#endif // DISTANCETERM_H
