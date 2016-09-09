#include <SEL/Object.h>
#include <SEL/SearchCondition.h>
#include <SEL/BooleanValue.h>

#include <algorithm>

using namespace SEL;

Object::Object(std::list<Qualifier*> qualList, QString objName)
    : mQualList(std::move(qualList))
    , mObjName(std::move(objName))
{
}

Object::Object(const Object& other)
    : mQualList(other.mQualList)
    , mObjName(other.mObjName)
{
}

Object& Object::operator=(const Object& other) {
    for(auto it = mQualList.begin(); it != mQualList.end(); ++it)
        delete *it;

    mQualList = other.mQualList;
    mObjName = other.mObjName;

    return *this;
}

Object::~Object() {
    for(auto it = mQualList.begin(); it != mQualList.end(); ++it)
        delete *it;
}

QString Object::getName() const {
    return mObjName;
}

std::vector<std::shared_ptr<SceneObject>> Object::getSceneObjects(const SearchCondition& searchCond, SceneObjectManager* sceneObjMgr, int sceneIdx,
                                                                  const DatasetManager::LabelMap& labels, bool applyQualifiers) const {
    // Get all objects matching the specified names
    std::vector<std::shared_ptr<SceneObject>> ret;
    unsigned int actualSceneIdx = (sceneIdx >= 0 ? static_cast<unsigned int>(sceneIdx) : sceneObjMgr->getCurrentSceneIdx());
    RGBDScene* rgbdScene = sceneObjMgr->getRGBDScene(actualSceneIdx);

    DatasetManager::LabelMap::const_iterator label = labels.find(mObjName);
    if(label != labels.cend()) {
        // Get all pixels covered by this label
        cv::Mat labelImg = sceneObjMgr->getScene(actualSceneIdx).getLabelImg();
        RegionMap regionMap(
            [](const cv::Point& lhs, const cv::Point& rhs) -> bool {
                return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
            }
        );

        for(cv::Mat_<unsigned short>::const_iterator jt = labelImg.begin<unsigned short>(); jt != labelImg.end<unsigned short>(); ++jt) {
            if(*jt == *label) regionMap.insert(std::make_pair(jt.pos(), -1));
        }

        // Perform region growing to separate objects of same type
        ret = doRegionGrowing(labelImg, regionMap, rgbdScene->getSceneMgr(), actualSceneIdx);

        // Remove objects that are not in the scene any more or for which the search condition doesn't hold
        std::vector<std::shared_ptr<SceneObject>>::iterator retNewEnd = std::remove_if(ret.begin(), ret.end(), [&](const std::shared_ptr<SceneObject>& obj) -> bool {
            return !sceneObjMgr->checkObjectInScene(*obj) || !searchCond.eval(sceneObjMgr, sceneIdx, *obj, labels);
        });
        ret.erase(retNewEnd, ret.end());

        // Add objects which have been moved before
        // Check name and search condition for them as well
        SceneObjectManager::ObjVec regObjects = sceneObjMgr->getRegisteredObjects(actualSceneIdx);
        SceneObjectManager::ObjVec::iterator regObjectsNewEnd = std::remove_if(regObjects.begin(), regObjects.end(),
            [&](const std::pair<SceneObjectManager::SceneObjPtr, Ogre::SceneNode*>& obj) -> bool {
                return obj.first->getName() != mObjName || !searchCond.eval(sceneObjMgr, sceneIdx, *obj.first, labels);
            }
        );

        ret.reserve(ret.size() + std::distance(regObjects.begin(), regObjectsNewEnd));
        for(SceneObjectManager::ObjVec::iterator it = regObjects.begin(); it != regObjectsNewEnd; ++it) {
            ret.push_back(it->first);
        }

        // Apply qualifiers
        if(applyQualifiers) {
            for(auto&& qualPtr : mQualList) {
                if(!applyQualifier(*qualPtr, ret))
                    std::cerr << "Unable to apply qualifier " << qualPtr->getText().toStdString() << " to " << mObjName.toStdString() << std::endl;
            }
        }
    }

    return ret;
}

std::vector<std::shared_ptr<SceneObject>> Object::getSceneObjects(SceneObjectManager* sceneObjMgr, int sceneIdx,
                                                                  const DatasetManager::LabelMap& labels, bool applyQualifiers) const {
    // Create temporary search condition which always evaluates to true
    SearchCondition tmpCond(nullptr, new BooleanTerm(nullptr, new BooleanFactor(new BooleanTest(new BooleanValue(true)), false)));

    // Call overloaded function
    return getSceneObjects(tmpCond, sceneObjMgr, sceneIdx, labels, applyQualifiers);
}

std::vector<std::shared_ptr<SceneObject>> Object::doRegionGrowing(const cv::Mat& labelImg, RegionMap& points, Ogre::SceneManager* sceneMgr, unsigned int sceneIdx) const {
    std::vector<std::shared_ptr<SceneObject>> ret;

    int currentId = 0;
    std::queue<RegionMap::iterator> queue;

    // Iterate over all inliers
    for(RegionMap::iterator it = points.begin(); it != points.end(); ++it) {
        // Check if this point was already processed
        if(it->second != -1)
            continue;

        // Add point to the queue
        queue.push(it);

        // Create new object
        ret.push_back(std::make_shared<SceneObject>(mObjName, sceneIdx, labelImg.size(), sceneMgr));

        // Iterate until the whole region is marked
        while(!queue.empty()) {
            RegionMap::iterator current = queue.front();
            queue.pop();

            if(current->second != -1)
                continue;

            // This point is unmarked, so mark it with the current region ID
            current->second = currentId;

            const cv::Point& pixel = current->first;
            ret[currentId]->addPoint(pixel);

            // Add all points to the queue that are direct neighbors (8-connected grid) of the current point
            // and have the same label
            for(int y = pixel.y - 1; y <= pixel.y + 1; ++y) {
                for(int x = pixel.x - 1; x <= pixel.x + 1; ++x) {
                    RegionMap::iterator neighbor = points.find(cv::Point(x, y));
                    // The current point itself will not be added since it is no longer marked with -1
                    if(neighbor != points.end() && neighbor->second == -1
                            && labelImg.at<unsigned short>(neighbor->first) == labelImg.at<unsigned short>(pixel)) {
                        queue.push(neighbor);
                    }
                }
            }
        }

        ++currentId;
    }

    return ret;
}

bool Object::applyQualifier(const Qualifier& qual, std::vector<std::shared_ptr<SceneObject>>& objList) const {
    // TODO: implement
    (void)qual;
    (void)objList;
    return false;
}

Object* Object::clone() const {
    return new Object(*this);
}

void Object::print(std::ostream& os) const {
    os << "Object " << mObjName.toStdString() << " with " << mQualList.size() << " qualifiers";
}
