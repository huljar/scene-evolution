#ifndef SCENE_H
#define SCENE_H

#include <opencv2/core/core.hpp>

#include <QString>

class Scene
{
public:
    Scene();
    Scene(const cv::Mat& depthImg, const cv::Mat& rgbImg, const cv::Mat& labelImg, const QString& fileName = "");

    cv::Mat getDepthImg() const;
    cv::Mat getRgbImg() const;
    cv::Mat getLabelImg() const;

    QString getFileName() const;

protected:
    cv::Mat mDepthImg;
    cv::Mat mRgbImg;
    cv::Mat mLabelImg;

    QString mFileName;

};

#endif // SCENE_H
