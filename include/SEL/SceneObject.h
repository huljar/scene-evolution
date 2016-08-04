#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <scene-evolution/CameraManager.h>

#include <opencv2/core/core.hpp>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreSceneManager.h>
#include <QString>

namespace SEL {
    class SceneObject
    {
    public:
        SceneObject();
        SceneObject(const QString& objName, const cv::Size& imgSize, Ogre::SceneManager* sceneMgr);
        virtual ~SceneObject();

        SceneObject(SceneObject&& other);
        SceneObject& operator=(SceneObject&& other);

        void addPoint(const cv::Point& point);

        bool meshify(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr);

        cv::Vec2f getCentroid2D() const;
        cv::Vec3f getCentroid3D(const cv::Mat& depthImg, const CameraManager& camMgr) const;

        QString getName() const;

        bool hasManualObject() const;
        Ogre::ManualObject* getManualObject() const;

        cv::Mat_<unsigned char> getPixels() const;

    protected:
        QString mObjName;
        cv::Mat_<unsigned char> mPixels;

        Ogre::ManualObject* mManualObject;
        Ogre::SceneManager* mSceneMgr;

    private:
        typedef std::map<cv::Point, Ogre::uint32, bool(*)(const cv::Point&, const cv::Point&)> IndexMap;

        IndexMap createVertices(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr);
        void createIndices(const IndexMap& idxMap);
    };
}

#endif // SCENEOBJECT_H
