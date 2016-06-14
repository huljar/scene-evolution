#include <scene-evolution/Scene.h>

Scene::Scene()
    : mDepthImg(cv::Mat())
    , mRgbImg(cv::Mat())
    , mLabelImg(cv::Mat())
    , mFileName("")
{

}

Scene::Scene(const cv::Mat& depthImg, const cv::Mat& rgbImg, const cv::Mat& labelImg, const QString& fileName)
    : mDepthImg(depthImg)
    , mRgbImg(rgbImg)
    , mLabelImg(labelImg)
    , mFileName(fileName)
{
}

cv::Mat Scene::getDepthImg() const {
    return mDepthImg;
}

cv::Mat Scene::getRgbImg() const {
    return mRgbImg;
}

cv::Mat Scene::getLabelImg() const {
    return mLabelImg;
}

QString Scene::getFileName() const {
    return mFileName;
}
