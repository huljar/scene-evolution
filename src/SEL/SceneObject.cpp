#include <SEL/SceneObject.h>
#include <scene-evolution/util.h>

using namespace SEL;

SceneObject::SceneObject()
    : mManualObject(nullptr)
    , mSceneMgr(nullptr)
{
}

SceneObject::SceneObject(const QString& objName, const cv::Size& imgSize, Ogre::SceneManager* sceneMgr)
    : mObjName(objName)
    , mPixels(cv::Mat_<unsigned char>::zeros(imgSize))
    , mManualObject(nullptr)
    , mSceneMgr(sceneMgr)
{
}

SceneObject::~SceneObject() {
    if(mSceneMgr && mManualObject) {
        mManualObject->detachFromParent();
        mSceneMgr->destroyManualObject(mManualObject);
    }
}

SceneObject::SceneObject(SceneObject&& other)
    : mObjName(std::move(other.mObjName))
    , mPixels(std::move(other.mPixels))
    , mManualObject(other.mManualObject)
    , mSceneMgr(other.mSceneMgr)
{
    other.mManualObject = nullptr;
    other.mSceneMgr = nullptr;
}

SceneObject& SceneObject::operator=(SceneObject&& other) {
    if(mSceneMgr && mManualObject) {
        mManualObject->detachFromParent();
        mSceneMgr->destroyManualObject(mManualObject);
    }

    mObjName = std::move(other.mObjName);
    mPixels = std::move(other.mPixels);
    mManualObject = other.mManualObject;
    mSceneMgr = other.mSceneMgr;

    other.mManualObject = nullptr;
    other.mSceneMgr = nullptr;

    return *this;
}

void SceneObject::addPoint(const cv::Point& point) {
    mPixels(point) = 255;
}

bool SceneObject::meshify(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr) {
    if(!mSceneMgr || mPixels.rows != depthImg.rows || mPixels.cols != depthImg.cols || mPixels.rows != rgbImg.rows || mPixels.cols != rgbImg.cols)
        return false;

    if(!mManualObject)
        mManualObject = mSceneMgr->createManualObject();
    else
        mManualObject->clear();

    mManualObject->begin(Strings::StandardMaterialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
    IndexMap idxMap = createVertices(depthImg, rgbImg, camMgr);
    createIndices(idxMap);
    mManualObject->end();

    return true;
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

cv::Vec3f SceneObject::getCentroid3D(const cv::Mat& depthImg, const CameraManager& camMgr) const {
    cv::Vec3f ret(0.0, 0.0, 0.0);
    unsigned int count = 0;

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            ret += camMgr.getWorldForDepth(it.pos(), depthImg.at<unsigned short>(it.pos()));
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

bool SceneObject::hasManualObject() const {
    return mManualObject != nullptr;
}

Ogre::ManualObject* SceneObject::getManualObject() const {
    return mManualObject;
}

cv::Mat_<unsigned char> SceneObject::getPixels() const {
    return mPixels;
}

SceneObject::IndexMap SceneObject::createVertices(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr) {
    unsigned int idx = 0;
    IndexMap idxMap(
        [](const cv::Point& lhs, const cv::Point& rhs) -> bool {
            return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
        }
    );

    for(int v = 0; v < mPixels.rows; ++v) {
        for(int u = 0; u < mPixels.cols; ++u) {
            if(mPixels(v, u) == 255) {
                // Transform depth pixel to world coordinates
                cv::Vec3f worldPoint = camMgr.getWorldForDepth(u, v, depthImg.at<unsigned short>(v, u));
                mManualObject->position(worldPoint[0], worldPoint[1], worldPoint[2]);

                // Retrieve corresponding RGB pixel
                cv::Point rgbPx = camMgr.getRGBForDepth(u, v, depthImg.at<unsigned short>(v, u));
                cv::Vec3b rgbColor = rgbImg.at<cv::Vec3b>(rgbPx);

                // Ogre uses RGB and OpenCV uses BGR, hence the reversed indexing
                mManualObject->colour(static_cast<float>(rgbColor[2]) / 255.0f,
                                      static_cast<float>(rgbColor[1]) / 255.0f,
                                      static_cast<float>(rgbColor[0]) / 255.0f);

                // Insert into map
                idxMap.insert(std::make_pair(cv::Point(u, v), idx));
                ++idx;
            }
        }
    }

    return idxMap;
}

void SceneObject::createIndices(const IndexMap& idxMap) {
    for(int v = 0; v < mPixels.rows - 1; ++v) {
        for(int u = 0; u < mPixels.cols - 1; ++u) {
            // Check mask
            if(mPixels(v, u) == 255 && mPixels(v + 1, u) == 255 && mPixels(v, u + 1) == 255 && mPixels(v + 1, u + 1) == 255) {
                // Create 2 triangles (= 1 "square") per iteration
                mManualObject->index(idxMap.find(cv::Point(u, v))->second);
                mManualObject->index(idxMap.find(cv::Point(u, v + 1))->second);
                mManualObject->index(idxMap.find(cv::Point(u + 1, v))->second);

                mManualObject->index(idxMap.find(cv::Point(u + 1, v))->second);
                mManualObject->index(idxMap.find(cv::Point(u, v + 1))->second);
                mManualObject->index(idxMap.find(cv::Point(u + 1, v + 1))->second);
            }
        }
    }
}
