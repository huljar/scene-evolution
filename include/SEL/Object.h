#ifndef OBJECT_H
#define OBJECT_H

#include <SEL/BisonSymbol.h>
#include <SEL/Qualifier.h>
#include <SEL/SceneObject.h>

#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/DatasetManager.h>

#include <QString>
#include <list>
#include <memory>
#include <vector>

namespace SEL {
    class SearchCondition; // forward declaration

    class Object : public BisonSymbol
    {
    public:
        Object(std::list<Qualifier*> qualList, QString objName);
        Object(const Object& other);
        Object& operator=(const Object& other);
        virtual ~Object();

        virtual Object* clone() const;

        QString getName() const;

        virtual std::vector<std::shared_ptr<SceneObject>> getSceneObjects(const SearchCondition& searchCond, SceneObjectManager* sceneObjMgr, int sceneIdx,
                                                                          const DatasetManager::LabelMap& labels, bool applyQualifiers = true) const;
        virtual std::vector<std::shared_ptr<SceneObject>> getSceneObjects(SceneObjectManager* sceneObjMgr, int sceneIdx,
                                                                          const DatasetManager::LabelMap& labels, bool applyQualifiers = true) const;

    protected:
        typedef std::map<cv::Point, int, bool(*)(const cv::Point&, const cv::Point&)> RegionMap; // need std::map to define comparison

        std::vector<std::shared_ptr<SceneObject>> doRegionGrowing(const cv::Mat& labelImg, RegionMap& points, Ogre::SceneManager* sceneMgr, unsigned int sceneIdx) const;

        virtual bool applyQualifier(const Qualifier& qual, std::vector<std::shared_ptr<SceneObject>>& objList) const;

        virtual void print(std::ostream& os) const;

        std::list<Qualifier*> mQualList;
        QString mObjName;
    };
}

#endif // OBJECT_H
