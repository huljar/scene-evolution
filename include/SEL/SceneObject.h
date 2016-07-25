#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <scene-evolution/CameraManager.h>

#include <opencv2/core/core.hpp>

#include <QString>

namespace SEL {
    class SceneObject
    {
    public:
        SceneObject();
        SceneObject(const QString& objName, const cv::Size& imgSize);

        void addPoint(const cv::Point& point);

        cv::Vec2f getCentroid2D() const;
        cv::Vec3f getCentroid3D(const cv::Mat& labelImg, const CameraManager& camMgr) const;

    protected:
        QString mObjName;
        cv::Mat_<unsigned char> mPixels;
    };
}

#endif // SCENEOBJECT_H
