#include <SEL/SceneObject.h>

using namespace SEL;

SceneObject::SceneObject()
    : mManualObject(nullptr)
{
}

SceneObject::SceneObject(const QString& objName, const cv::Size& imgSize)
    : mObjName(objName)
    , mPixels(cv::Mat_<unsigned char>::zeros(imgSize))
    , mManualObject(nullptr)
{
}

void SceneObject::addPoint(const cv::Point& point) {
    mPixels(point) = 255;
}

cv::Vec2f SceneObject::getCentroid2D() const {
    cv::Vec2f ret(0.0, 0.0);
    unsigned int count = 0;

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            ret[0] += static_cast<float>(it.pos().x);
            ret[1] += static_cast<float>(it.pos().y);
            ++count;
        }
    }

    ret[0] /= static_cast<float>(count);
    ret[1] /= static_cast<float>(count);

    return ret;
}

cv::Vec3f SceneObject::getCentroid3D(const cv::Mat& labelImg, const CameraManager& camMgr) const {
    cv::Vec3f ret(0.0, 0.0, 0.0);
    unsigned int count = 0;

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            ret += camMgr.getWorldForDepth(it.pos(), labelImg.at<unsigned short>(it.pos()));
            ++count;
        }
    }

    for(unsigned int i = 0; i < 3; ++i)
        ret[i] /= static_cast<float>(count);

    return ret;
}

QString SceneObject::getName() const {
    return mObjName;
}
