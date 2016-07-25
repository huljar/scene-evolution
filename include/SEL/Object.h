#ifndef OBJECT_H
#define OBJECT_H

#include <SEL/BisonSymbol.h>
#include <SEL/Qualifier.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/Scene.h>

#include <QMap>
#include <QString>
#include <QVector>
#include <list>

namespace SEL {
    class SearchCondition; // forward declaration

    class Object : public BisonSymbol
    {
    public:
        typedef QMap<QString, unsigned short> LabelMap;

        Object(std::list<Qualifier*> qualList, QString objName);
        Object(const Object& other);
        Object& operator=(const Object& other);
        virtual ~Object();

        virtual Object* clone() const;

        QString getName() const;

        QVector<SceneObject> getSceneObjects(const SearchCondition& searchCond, RGBDScene* rgbdScene, const Scene& currentScene,
                                             const LabelMap& labels, bool applyQualifiers = true) const;
        QVector<SceneObject> getSceneObjects(RGBDScene* rgbdScene, const Scene& currentScene,
                                             const LabelMap& labels, bool applyQualifiers = true) const;

    protected:
        typedef std::map<cv::Point, int, bool(*)(const cv::Point&, const cv::Point&)> RegionMap; // need std::map to define comparison

        QVector<unsigned int> doRegionGrowing(const cv::Mat& labelImg, RegionMap& points) const;

        bool applyQualifier(const Qualifier& qual, QVector<SceneObject>& objList) const;

        virtual void print(std::ostream& os) const;

        std::list<Qualifier*> mQualList;
        QString mObjName;
    };
}

#endif // OBJECT_H
