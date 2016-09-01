/************************************************************//**
 * @file
 *
 * @brief RGBDScene class header file.
 *
 * @author Julian Harttung
 *//************************************************************/

#ifndef RGBDSCENE_H
#define RGBDSCENE_H

#include <scene-evolution/CameraManager.h>
#include <SEL/SceneObject.h>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgrePlatform.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector2.h>

#include <opencv2/core/core.hpp>

/**
 * @brief Class that represents an RGBD scene constructed from a depth and an RGB image.
 *
 * This class manages building the 3D mesh from a depth and an RGB image and providing it to OGRE.
 */
class RGBDScene
{
public:
    RGBDScene(Ogre::SceneManager* sceneManager, const cv::Mat& depthImage, const cv::Mat& rgbImage,
              const CameraManager& cameraParams, bool autoCreateMesh = true);
    virtual ~RGBDScene();

    // Move semantics (defining those automatically prevents generation of default copy constructor and copy assignment operator)
    RGBDScene(RGBDScene&& other);
    RGBDScene& operator=(RGBDScene&& other);

    void meshify(const cv::Mat_<unsigned char>& mask = cv::Mat_<unsigned char>(), bool mergeWithOldMask = true);

    bool screenspaceCoords(Ogre::Vector2& resultTopLeft, Ogre::Vector2& resultBottomRight, const Ogre::Camera* camera = nullptr) const;

    Ogre::ManualObject* getManualObject() const;

    cv::Mat getDepthImage() const;
    cv::Mat getRGBImage() const;

    CameraManager getCameraManager() const;
    CameraManager& cameraManager();

    Ogre::SceneManager* getSceneMgr() const;

    cv::Mat_<unsigned char> getCurrentMask() const;

protected:
    Ogre::ManualObject* mSceneObject;
    Ogre::SceneManager* mSceneMgr;

    cv::Mat mDepthImage;
    cv::Mat mRGBImage;

    CameraManager mCameraManager;

    cv::Mat_<unsigned char> mCurrentMask;

private:
    void createVertices();
    void createIndices();

    inline Ogre::uint32 pixelToIndex(int x, int y) const { return y * mRGBImage.cols + x; }
};

#endif // RGBDSCENE_H
