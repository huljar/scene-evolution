#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <scene-evolution/CameraManager.h>
#include <scene-evolution/KDTree.h>

#include <opencv2/core/core.hpp>
#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreSceneManager.h>
#include <QString>

namespace SEL {
    class SceneObject
    {
    public:
        typedef std::pair<Ogre::Vector3, float> Neighbor;

        SceneObject();
        SceneObject(const QString& objName, const cv::Size& imgSize, Ogre::SceneManager* sceneMgr);
        virtual ~SceneObject();

        SceneObject(SceneObject&& other);
        SceneObject& operator=(SceneObject&& other);

        void addPoint(const cv::Point& point);
        void removePoint(const cv::Point& point);

        bool meshify(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr);

        Ogre::AxisAlignedBox getBoundingBox(const cv::Mat& depthImg, const CameraManager& camMgr);

        QString getName() const;

        bool hasManualObject() const;
        Ogre::ManualObject* getManualObject() const;

        cv::Mat_<unsigned char> getOriginalPixels() const;
        cv::Mat_<cv::Vec3f> getOriginalPixels3D(const cv::Mat& depthImg, const CameraManager& camMgr);

        std::vector<Ogre::Vector3> getVertices(const cv::Mat& depthImg, const CameraManager& camMgr);

        std::vector<Neighbor> findKNearestNeighbors(const cv::Vec3f& origin, unsigned int k, const cv::Mat& depthImg, const CameraManager& camMgr);
        std::vector<Neighbor> findKNearestNeighbors(const Ogre::Vector3& origin, unsigned int k, const cv::Mat& depthImg, const CameraManager& camMgr);

        cv::Vec3f getCurrentTranslation() const;
        void setCurrentTranslation(const cv::Vec3f& translation);
        void setCurrentTranslation(float x, float y, float z);
        void translate(const cv::Vec3f& translation);
        void translate(float x, float y, float z);

        cv::Matx33f getCurrentRotation() const;
        void setCurrentRotation(const cv::Matx33f& rotation);
        void setCurrentRotation(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22);

        cv::Vec3f getCurrentScale() const;
        void setCurrentScale(const cv::Vec3f& scale);
        void setCurrentScale(float x, float y, float z);

    protected:
        QString mObjName;
        cv::Mat_<unsigned char> mPixels;
        cv::Mat_<cv::Vec3f> m3DPixels;

        cv::Vec3f mCurrentTranslation;
        cv::Matx33f mCurrentRotation;
        cv::Vec3f mCurrentScale;

        Ogre::ManualObject* mManualObject;
        Ogre::SceneManager* mSceneMgr;

        KDTree mKDTree;
        bool mKDTreeUpdated;
        std::vector<Ogre::Vector3> mKDTreeVertexList;
        bool mKDTreeVertexListUpdated;

    private:
        typedef std::map<cv::Point, Ogre::uint32, bool(*)(const cv::Point&, const cv::Point&)> IndexMap;

        IndexMap createVertices(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr, cv::Vec3f* centroid = nullptr);
        void createIndices(const IndexMap& idxMap);

        bool has3DPixels() const;
        void create3DPixels(const cv::Mat& depthImg, const CameraManager& camMgr);

        void updateKDTreeVertexList(const cv::Mat& depthImg, const CameraManager& camMgr);
    };
}

#endif // SCENEOBJECT_H
