#include <scene-evolution/CameraManager.h>

#include <cmath>

CameraManager::CameraManager(const cv::Vec2f& depthPrincipalPoint, const cv::Vec2f& depthFocalLength,
                             const cv::Vec2f& rgbPrincipalPoint, const cv::Vec2f& rgbFocalLength,
                             const cv::Matx33f& rotation, const cv::Vec3f translation,
                             MapMode mapMode)
    : mDepthPrincipalPoint(depthPrincipalPoint)
    , mDepthFocalLength(depthFocalLength)
    , mRGBPrincipalPoint(rgbPrincipalPoint)
    , mRGBFocalLength(rgbFocalLength)
    , mRotation(rotation)
    , mTranslation(translation)
    , mMapMode(mapMode)
{
}

cv::Vec3f CameraManager::getWorldForDepth(const cv::Point& uv, unsigned short d) const {
    return getWorldForDepth(uv.x, uv.y, d);
}

cv::Vec3f CameraManager::getWorldForDepth(int u, int v, unsigned short d) const {
    const cv::Vec2f& principalPoint = (mMapMode == MapMode::MAPPED_DEPTH_TO_RGB ? mRGBPrincipalPoint : mDepthPrincipalPoint);
    const cv::Vec2f& focalLength = (mMapMode == MapMode::MAPPED_DEPTH_TO_RGB ? mRGBFocalLength : mDepthFocalLength);

    float uf = static_cast<float>(u),
          vf = static_cast<float>(v),
          df = static_cast<float>(d);

    float x = (uf - principalPoint[0]) * df / focalLength[0];
    float y = -((vf - principalPoint[1]) * df / focalLength[1]);
    float z = -df;

    return cv::Vec3f(x, y, z);
}

cv::Point CameraManager::getDepthForWorld(const cv::Vec3f& xyz) const {
    return getDepthForWorld(xyz[0], xyz[1], xyz[2]);
}

cv::Point CameraManager::getDepthForWorld(float x, float y, float z) const {
    cv::Vec2f principalPoint = (mMapMode == MapMode::MAPPED_DEPTH_TO_RGB ? mRGBPrincipalPoint : mDepthPrincipalPoint);
    cv::Vec2f focalLength = (mMapMode == MapMode::MAPPED_DEPTH_TO_RGB ? mRGBFocalLength : mDepthFocalLength);

    int u = std::round(x * focalLength[0] / (-z) + principalPoint[0]);
    int v = std::round((-y) * focalLength[1] / (-z) + principalPoint[1]);

    return cv::Point(u, v);
}

cv::Point CameraManager::getRGBForWorld(const cv::Vec3f& xyz) const {
    cv::Vec3f xyzNew;
    cv::Vec2f principalPoint = (mMapMode == MapMode::MAPPED_RGB_TO_DEPTH ? mDepthPrincipalPoint : mRGBPrincipalPoint);
    cv::Vec2f focalLength = (mMapMode == MapMode::MAPPED_RGB_TO_DEPTH ? mDepthPrincipalPoint : mRGBPrincipalPoint);

    if(mMapMode == MapMode::MAPPED_DEPTH_TO_RGB || mMapMode == MapMode::MAPPED_RGB_TO_DEPTH)
        xyzNew = xyz; // no transformation needed
    else if(mMapMode == MapMode::UNMAPPED_DEPTH_TO_RGB)
        xyzNew = mRotation.t() * (xyz - mTranslation); // apply inverse transformation
    else // mMapMode == MapMode::UNMAPPED_RGB_TO_DEPTH
        xyzNew = mRotation * xyz + mTranslation; // apply normal transformation

    float x = xyzNew[0],
          y = xyzNew[1],
          z = xyzNew[2];

    int u = std::round(x * focalLength[0] / (-z) + principalPoint[0]);
    int v = std::round((-y) * focalLength[1] / (-z) + principalPoint[1]);

    return cv::Point(u, v);
}

cv::Point CameraManager::getRGBForWorld(float x, float y, float z) const {
    return getRGBForWorld(cv::Vec3f(x, y, z));
}

cv::Point CameraManager::getLabelForWorld(const cv::Vec3f& xyz) const {
    return getLabelForWorld(xyz[0], xyz[1], xyz[2]);
}

cv::Point CameraManager::getLabelForWorld(float x, float y, float z) const {
    return getDepthForWorld(x, y, z);
}

cv::Point CameraManager::getRGBForDepth(const cv::Point& uv, unsigned short d) const {
    return getRGBForDepth(uv.x, uv.y, d);
}

cv::Point CameraManager::getRGBForDepth(int u, int v, unsigned short d) const {
    if(mMapMode == MapMode::MAPPED_DEPTH_TO_RGB || mMapMode == MapMode::MAPPED_RGB_TO_DEPTH)
        return cv::Point(u, v);

    return getRGBForWorld(getWorldForDepth(u, v, d));
}

cv::Point CameraManager::getLabelForDepth(const cv::Point& uv, unsigned short d) const {
    return getLabelForDepth(uv.x, uv.y, d);
}

cv::Point CameraManager::getLabelForDepth(int u, int v, unsigned short d) const {
    (void)d;
    return cv::Point(u, v);
}

cv::Vec2f CameraManager::getDepthPrincipalPoint() const {
    return mDepthPrincipalPoint;
}

cv::Vec2f CameraManager::getDepthFocalLength() const {
    return mDepthFocalLength;
}

cv::Vec2f CameraManager::getRGBPrincipalPoint() const {
    return mRGBPrincipalPoint;
}

cv::Vec2f CameraManager::getRGBFocalLength() const {
    return mRGBFocalLength;
}

cv::Matx33f CameraManager::getRotation() const {
    return mRotation;
}

cv::Vec3f CameraManager::getTranslation() const {
    return mTranslation;
}

CameraManager::MapMode CameraManager::getMapMode() const {
    return mMapMode;
}
