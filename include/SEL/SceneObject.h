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
        typedef std::pair<cv::Point, cv::Point> BoundingBox2D;
        typedef std::pair<cv::Vec3f, cv::Vec3f> BoundingBox3D;

        SceneObject();
        SceneObject(const QString& objName, const cv::Size& imgSize, Ogre::SceneManager* sceneMgr);
        virtual ~SceneObject();

        SceneObject(SceneObject&& other);
        SceneObject& operator=(SceneObject&& other);

        void addPoint(const cv::Point& point);

        bool meshify(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr);

        cv::Vec2f getCentroid2D() const;
        cv::Vec3f getCentroid3D(const cv::Mat& depthImg, const CameraManager& camMgr);

        BoundingBox2D getBoundingBox2D() const;
        BoundingBox3D getBoundingBox3D(const cv::Mat& depthImg, const CameraManager& camMgr);

        QString getName() const;

        bool hasManualObject() const;
        Ogre::ManualObject* getManualObject() const;

        cv::Mat_<unsigned char> getPixels() const;

        bool has3DPixels() const;
        void create3DPixels(const cv::Mat& depthImg, const CameraManager& camMgr);
        void invalidate3DPixels();
        cv::Mat_<cv::Vec3f> get3DPixels() const;

        cv::Vec3f getCurrentTranslation() const;
        void setCurrentTranslation(const cv::Vec3f& currentTranslation);
        void setCurrentTranslation(float x, float y, float z);

        cv::Matx33f getCurrentRotation() const;
        void setCurrentRotation(const cv::Matx33f& currentRotation);
        void setCurrentRotation(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22);

    protected:
        QString mObjName;
        cv::Mat_<unsigned char> mPixels;
        cv::Mat_<cv::Vec3f> m3DPixels;

        cv::Vec3f mCurrentTranslation;
        cv::Matx33f mCurrentRotation;

        Ogre::ManualObject* mManualObject;
        Ogre::SceneManager* mSceneMgr;

    private:
        typedef std::map<cv::Point, Ogre::uint32, bool(*)(const cv::Point&, const cv::Point&)> IndexMap;

        IndexMap createVertices(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr);
        void createIndices(const IndexMap& idxMap);
    };
}

#endif // SCENEOBJECT_H
