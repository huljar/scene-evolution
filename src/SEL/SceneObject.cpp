#include <SEL/SceneObject.h>
#include <scene-evolution/util.h>

#include <limits>

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
    , m3DPixels(std::move(other.m3DPixels))
    , mCurrentTranslation(std::move(other.mCurrentTranslation))
    , mCurrentRotation(std::move(other.mCurrentRotation))
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
    m3DPixels = std::move(other.m3DPixels);
    mCurrentTranslation = std::move(other.mCurrentTranslation);
    mCurrentRotation = std::move(other.mCurrentRotation);
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
    if(!mSceneMgr || mPixels.rows != depthImg.rows || mPixels.cols != depthImg.cols || mPixels.rows != rgbImg.rows || mPixels.cols != rgbImg.cols) {
        std::cout << "Error: invalid dimensions, cannot meshify object" << std::endl;
        return false;
    }

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

cv::Vec3f SceneObject::getCentroid3D(const cv::Mat& depthImg, const CameraManager& camMgr) {
    cv::Vec3f ret(0.0, 0.0, 0.0);
    unsigned int count = 0;

    if(!has3DPixels()) create3DPixels(depthImg, camMgr);

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            ret += m3DPixels(it.pos());
            ++count;
        }
    }

    for(unsigned int i = 0; i < 3; ++i)
        ret[i] /= static_cast<float>(count);

    return ret;
}

SceneObject::BoundingBox2D SceneObject::getBoundingBox2D() const {
    cv::Point smallest(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    cv::Point largest(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            cv::Point pos = it.pos();
            if(pos.x < smallest.x) smallest.x = pos.x;
            if(pos.y < smallest.y) smallest.y = pos.y;
            if(pos.x > largest.x) largest.x = pos.x;
            if(pos.y > largest.y) largest.y = pos.y;
        }
    }

    return std::make_pair(smallest, largest);
}

SceneObject::BoundingBox3D SceneObject::getBoundingBox3D(const cv::Mat& depthImg, const CameraManager& camMgr) {
    cv::Vec3f smallest(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    cv::Vec3f largest(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

    if(!has3DPixels()) create3DPixels(depthImg, camMgr);

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            cv::Vec3f point = m3DPixels(it.pos());
            for(int i = 0; i < 3; ++i) {
                if(point[i] < smallest[i]) smallest[i] = point[i];
                if(point[i] > largest[i]) largest[i] = point[i];
            }
        }
    }

    return std::make_pair(smallest, largest);
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

bool SceneObject::has3DPixels() const {
    return m3DPixels.data;
}

void SceneObject::create3DPixels(const cv::Mat& depthImg, const CameraManager& camMgr) {
    m3DPixels.create(mPixels.size());

    for(cv::Mat_<unsigned char>::iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            // Transform depth pixel to world coordinates and insert into Mat
            m3DPixels(it.pos()) = camMgr.getWorldForDepth(it.pos(), depthImg.at<unsigned short>(it.pos()));
        }
    }
}

void SceneObject::invalidate3DPixels() {
    m3DPixels.release();
}

cv::Mat_<cv::Vec3f> SceneObject::get3DPixels() const {
    return m3DPixels;
}

cv::Vec3f SceneObject::getCurrentTranslation() const {
    return mCurrentTranslation;
}

void SceneObject::setCurrentTranslation(const cv::Vec3f& currentTranslation) {
    mCurrentTranslation = currentTranslation;
}

void SceneObject::setCurrentTranslation(float x, float y, float z) {
    setCurrentTranslation(cv::Vec3f(x, y, z));
}

cv::Matx33f SceneObject::getCurrentRotation() const {
    return mCurrentRotation;
}

void SceneObject::setCurrentRotation(const cv::Matx33f& currentRotation) {
    mCurrentRotation = currentRotation;
}

void SceneObject::setCurrentRotation(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22) {
    setCurrentRotation(cv::Matx33f(f00, f01, f02, f10, f11, f12, f20, f21, f22));
}

SceneObject::IndexMap SceneObject::createVertices(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr) {
    unsigned int idx = 0;
    IndexMap idxMap(
        [](const cv::Point& lhs, const cv::Point& rhs) -> bool {
            return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
        }
    );

    if(!has3DPixels()) create3DPixels(depthImg, camMgr);

    for(cv::Mat_<unsigned char>::iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            // Get 3D coordinates
            cv::Vec3f worldPos = m3DPixels(it.pos());
            mManualObject->position(worldPos[0], worldPos[1], worldPos[2]);

            // Retrieve corresponding RGB pixel
            cv::Point rgbPx = camMgr.getRGBForDepth(it.pos(), depthImg.at<unsigned short>(it.pos()));
            cv::Vec3b rgbColor = rgbImg.at<cv::Vec3b>(rgbPx);

            // Ogre uses RGB and OpenCV uses BGR, hence the reversed indexing
            mManualObject->colour(static_cast<float>(rgbColor[2]) / 255.0f,
                                  static_cast<float>(rgbColor[1]) / 255.0f,
                                  static_cast<float>(rgbColor[0]) / 255.0f);

            // Insert into map
            idxMap.insert(std::make_pair(it.pos(), idx));
            ++idx;
        }
    }

    return idxMap;
}

void SceneObject::createIndices(const IndexMap& idxMap) {
    for(cv::Mat_<unsigned char>::iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        cv::Point right = cv::Point(it.pos().x + 1, it.pos().y);
        cv::Point bot = cv::Point(it.pos().x, it.pos().y + 1);
        cv::Point rightbot = cv::Point(it.pos().x + 1, it.pos().y + 1);

        // Check mask
        if(*it == 255 && mPixels(right) == 255 && mPixels(bot) == 255 && mPixels(rightbot) == 255) {
            // Create 2 triangles (= 1 "square") per iteration
            mManualObject->index(idxMap.find(it.pos())->second);
            mManualObject->index(idxMap.find(bot)->second);
            mManualObject->index(idxMap.find(right)->second);

            mManualObject->index(idxMap.find(right)->second);
            mManualObject->index(idxMap.find(bot)->second);
            mManualObject->index(idxMap.find(rightbot)->second);
        }
    }
}
