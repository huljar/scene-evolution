#include <SEL/SceneObject.h>
#include <scene-evolution/util.h>
#include <scene-evolution/interop.h>

#include <OgreBites/OgreRay.h>

#include <limits>

using namespace SEL;
using namespace interop;

SceneObject::SceneObject()
    : mCurrentTranslation(0.f, 0.f, 0.f)
    , mCurrentRotation(cv::Matx33f::eye())
    , mCurrentScale(1.f, 1.f, 1.f)
    , mManualObject(nullptr)
    , mSceneMgr(nullptr)
    , mKDTreeUpdated(false)
    , mKDTreeVertexListUpdated(false)
{
}

SceneObject::SceneObject(const QString& objName, const cv::Size& imgSize, Ogre::SceneManager* sceneMgr)
    : mObjName(objName)
    , mPixels(cv::Mat_<unsigned char>::zeros(imgSize))
    , mCurrentTranslation(0.f, 0.f, 0.)
    , mCurrentRotation(cv::Matx33f::eye())
    , mCurrentScale(1.f, 1.f, 1.f)
    , mManualObject(nullptr)
    , mSceneMgr(sceneMgr)
    , mKDTreeUpdated(false)
    , mKDTreeVertexListUpdated(false)
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
    , mCurrentScale(std::move(other.mCurrentScale))
    , mManualObject(other.mManualObject)
    , mSceneMgr(other.mSceneMgr)
    , mKDTree(std::move(other.mKDTree))
    , mKDTreeUpdated(other.mKDTreeUpdated)
    , mKDTreeVertexList(std::move(other.mKDTreeVertexList))
    , mKDTreeVertexListUpdated(other.mKDTreeVertexListUpdated)
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
    mCurrentScale = std::move(other.mCurrentScale);
    mManualObject = other.mManualObject;
    mSceneMgr = other.mSceneMgr;
    mKDTree = std::move(other.mKDTree);
    mKDTreeUpdated = other.mKDTreeUpdated;
    mKDTreeVertexList = std::move(other.mKDTreeVertexList);
    mKDTreeVertexListUpdated = other.mKDTreeVertexListUpdated;

    other.mManualObject = nullptr;
    other.mSceneMgr = nullptr;

    return *this;
}

void SceneObject::addPoint(const cv::Point& point) {
    mPixels(point) = 255;
}

void SceneObject::removePoint(const cv::Point& point) {
    mPixels(point) = 0;
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
    cv::Vec3f centroid;
    IndexMap idxMap = createVertices(depthImg, rgbImg, camMgr, &centroid);
    createIndices(idxMap);
    mManualObject->end();

    setCurrentTranslation(centroid);

    return true;
}

Ogre::AxisAlignedBox SceneObject::getBoundingBox(const cv::Mat& depthImg, const CameraManager& camMgr) {
    if(hasManualObject()) {
        // Use existing object to determine bounds
        return mManualObject->getWorldBoundingBox();
    }

    // Calculate bounds from pixel information
    if(!has3DPixels()) create3DPixels(depthImg, camMgr);

    Ogre::Vector3 smallest(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    Ogre::Vector3 largest(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

    for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            cv::Vec3f point = m3DPixels(it.pos());
            for(size_t i = 0; i < 3; ++i) {
                if(point[i] < smallest[i]) smallest[i] = point[i];
                if(point[i] > largest[i]) largest[i] = point[i];
            }
        }
    }

    return Ogre::AxisAlignedBox(smallest, largest);
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

cv::Mat_<unsigned char> SceneObject::getOriginalPixels() const {
    return mPixels;
}

cv::Mat_<cv::Vec3f> SceneObject::getOriginalPixels3D(const cv::Mat& depthImg, const CameraManager& camMgr) {
    if(!has3DPixels()) create3DPixels(depthImg, camMgr);
    return m3DPixels;
}

std::vector<Ogre::Vector3> SceneObject::getVertices(const cv::Mat& depthImg, const CameraManager& camMgr) {
    if(!mKDTreeVertexListUpdated)
        updateKDTreeVertexList(depthImg, camMgr);

    return mKDTreeVertexList;
}

std::vector<SceneObject::Neighbor> SceneObject::findKNearestNeighbors(const cv::Vec3f& origin, unsigned int k, const cv::Mat& depthImg, const CameraManager& camMgr) {
    return findKNearestNeighbors(cvToOgre(origin), k, depthIg, camMgr);
}

std::vector<SceneObject::Neighbor> SceneObject::findKNearestNeighbors(const Ogre::Vector3& origin, unsigned int k, const cv::Mat& depthImg, const CameraManager& camMgr) {
    if(!mKDTreeVertexListUpdated)
        updateKDTreeVertexList(depthImg, camMgr);

    if(!mKDTreeUpdated)
        mKDTree.initialize(&mKDTreeVertexList, Constants::SceneObjectKDTreeMaxDepth, Constants::SceneObjectKDTreeMinLeafSize);

    std::vector<KDTree::ResultEntry> result = mKDTree.findKNearestNeighbors(origin, k);

    std::vector<Neighbor> ret;
    ret.reserve(result.size());

    for(auto&& elem : result) {
        ret.push_back(std::make_pair(*elem.vertex, elem.sqrDistance));
    }

    return ret;
}

cv::Vec3f SceneObject::getCurrentTranslation() const {
    return mCurrentTranslation;
}

void SceneObject::setCurrentTranslation(const cv::Vec3f& translation) {
    mCurrentTranslation = translation;
    mKDTreeVertexListUpdated = false;
}

void SceneObject::setCurrentTranslation(float x, float y, float z) {
    setCurrentTranslation(cv::Vec3f(x, y, z));
}

void SceneObject::translate(const cv::Vec3f& translation) {
    mCurrentTranslation += translation;
    mKDTreeVertexListUpdated = false;
}

void SceneObject::translate(float x, float y, float z) {
    translate(cv::Vec3f(x, y, z));
}

cv::Matx33f SceneObject::getCurrentRotation() const {
    return mCurrentRotation;
}

void SceneObject::setCurrentRotation(const cv::Matx33f& rotation) {
    mCurrentRotation = rotation;
    mKDTreeVertexListUpdated = false;
}

void SceneObject::setCurrentRotation(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22) {
    setCurrentRotation(cv::Matx33f(f00, f01, f02, f10, f11, f12, f20, f21, f22));
}

cv::Vec3f SceneObject::getCurrentScale() const {
    return mCurrentScale;
}

void SceneObject::setCurrentScale(const cv::Vec3f& scale) {
    mCurrentScale = scale;
    mKDTreeVertexListUpdated = false;
}

void SceneObject::setCurrentScale(float x, float y, float z) {
    setCurrentScale(cv::Vec3f(x, y, z));
}

SceneObject::IndexMap SceneObject::createVertices(const cv::Mat& depthImg, const cv::Mat& rgbImg, const CameraManager& camMgr, cv::Vec3f* centroid) {
    if(!has3DPixels()) create3DPixels(depthImg, camMgr);

    // Calculate centroid of object
    cv::Vec3f centroidTmp(0.f, 0.f, 0.f);
    unsigned int centroidCount = 0;

    for(cv::Mat_<unsigned char>::iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            centroidTmp += m3DPixels(it.pos());
            ++centroidCount;
        }
    }
    centroidTmp /= static_cast<float>(centroidCount);

    // Set output parameter if requested
    if(centroid)
        *centroid = centroidTmp;

    // Build vertices
    unsigned int idx = 0;
    IndexMap idxMap(
        [](const cv::Point& lhs, const cv::Point& rhs) -> bool {
            return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
        }
    );

    for(cv::Mat_<unsigned char>::iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
        if(*it == 255) {
            // Get 3D coordinates shifted by the centroid
            cv::Vec3f worldPos = m3DPixels(it.pos()) - centroidTmp;
            mManualObject->position(cvToOgre(worldPos));

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

void SceneObject::updateKDTreeVertexList(const cv::Mat& depthImg, const CameraManager& camMgr) {
    if(hasManualObject()) {
        // Use the manual object to retrieve the vertices in their current transformation
        Ogre::Vector3* vertices;
        size_t vertexCount;
        unsigned long* indices;
        size_t indexCount;

        if(mManualObject->isAttached()) {
            // If the object is attached to a scene node, use its transformation
            OgreBites::OgreRay::GetMeshInformation(mManualObject, vertexCount, vertices, indexCount, indices,
                                                   mManualObject->getParentNode()->_getDerivedPosition(),
                                                   mManualObject->getParentNode()->_getDerivedOrientation(),
                                                   mManualObject->getParentNode()->_getDerivedScale());
        }
        else {
            // If the object is not attached, use the stored transformation parameters
            OgreBites::OgreRay::GetMeshInformation(mManualObject, vertexCount, vertices, indexCount, indices,
                                                   cvToOgre(mCurrentTranslation),
                                                   cvToOgre(mCurrentRotation),
                                                   cvToOgre(mCurrentScale));
        }

        // Copy the vertices to the vertex list
        mKDTreeVertexList.assign(vertices, vertices + vertexCount);

        // Clean up mesh information
        delete[] vertices;
        delete[] indices;
    }
    else {
        // Ensure that 3D pixels exist
        if(!has3DPixels()) create3DPixels(depthImg, camMgr);

        // Clear previous content
        mKDTreeVertexList.clear();

        // Iterate through object pixels
        for(cv::Mat_<unsigned char>::const_iterator it = mPixels.begin(); it != mPixels.end(); ++it) {
            if(*it == 255) {
                mKDTreeVertexList.push_back(cvToOgre(m3DPixels(it.pos())));
            }
        }
    }

    mKDTreeVertexListUpdated = true;
    mKDTreeUpdated = false;
}
