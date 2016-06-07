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

    cv::Mat getLabelTextImg() const;
    void setLabelTextImg(const cv::Mat& labelTextImg);

    cv::Mat getSegmentImg() const;
    void setSegmentImg(const cv::Mat& segmentImg);

protected:
    cv::Mat mDepthImg;
    cv::Mat mRgbImg;
    cv::Mat mLabelImg;

    QString mFileName;

    cv::Mat mLabelTextImg;
    cv::Mat mSegmentImg;

};

#endif // SCENE_H
