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
    , mLabelImg(labelImg)
    , mFileName(fileName)
{
    // Add alpha channel to RGB image
    cv::Mat alpha(rgbImg.size(), CV_8U, cv::Scalar::all(255));
    cv::Mat img(rgbImg.size(), CV_8UC4);

    cv::Mat in[] = {rgbImg, alpha};
    int from_to[] = {0, 0, 1, 1, 2, 2, 3, 3};

    cv::mixChannels(in, 2, &img, 1, from_to, 4);

    mRgbImg = img;
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

cv::Mat Scene::getLabelTextImg() const {
    return mLabelTextImg;
}

void Scene::setLabelTextImg(const cv::Mat& labelTextImg) {
    mLabelTextImg = labelTextImg;
}

cv::Mat Scene::getSegmentImg() const {
    return mSegmentImg;
}

void Scene::setSegmentImg(const cv::Mat& segmentImg) {
    mSegmentImg = segmentImg;
}
