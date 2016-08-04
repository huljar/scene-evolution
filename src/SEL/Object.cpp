#include <SEL/Object.h>
#include <SEL/SearchCondition.h>

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
    std::cerr << "Deleting Object" << std::endl;
}

QString Object::getName() const {
    return mObjName;
}

std::vector<SceneObject> Object::getSceneObjects(const SearchCondition& searchCond, RGBDScene* rgbdScene, const Scene& currentScene,
                                                 const DatasetManager::LabelMap& labels, bool applyQualifiers) const {
    // Get all objects matching the specified names
    std::vector<SceneObject> ret;

    DatasetManager::LabelMap::const_iterator label = labels.find(mObjName);
    if(label != labels.cend()) {
        // Get all pixels covered by this label
        const cv::Mat& labelImg = currentScene.getLabelImg();
        RegionMap regionMap(
            [](const cv::Point& lhs, const cv::Point& rhs) -> bool {
                return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
            }
        );

        for(cv::Mat_<unsigned short>::const_iterator jt = labelImg.begin<unsigned short>(); jt != labelImg.end<unsigned short>(); ++jt) {
            if(*jt == *label) regionMap.insert(std::make_pair(jt.pos(), -1));
        }

        // Perform region growing to separate objects of same type
        ret = doRegionGrowing(labelImg, regionMap, rgbdScene->getSceneMgr());

        // Remove objects for which the search condition doesn't hold
        std::vector<SceneObject>::iterator newEnd = std::remove_if(ret.begin(), ret.end(), [&](const SceneObject& obj) -> bool {
            return !searchCond.eval(rgbdScene, currentScene, obj);
        });
        ret.erase(newEnd, ret.end());
    }

    // Apply qualifiers
    if(applyQualifiers) {
        for(std::list<Qualifier*>::const_iterator it = mQualList.cbegin(); it != mQualList.cend(); ++it) {
            if(!applyQualifier(**it, ret))
                std::cerr << "Unable to apply qualifier " << (*it)->getText().toStdString() << " to " << mObjName.toStdString() << std::endl;
        }
    }

    return ret;
}

std::vector<SceneObject> Object::getSceneObjects(RGBDScene* rgbdScene, const Scene& currentScene,
                                                 const DatasetManager::LabelMap& labels, bool applyQualifiers) const {

}

std::vector<SceneObject> Object::doRegionGrowing(const cv::Mat& labelImg, RegionMap& points, Ogre::SceneManager* sceneMgr) const {
    std::vector<SceneObject> sceneObjects;

    int currentId = 0;
    std::queue<RegionMap::iterator> queue;

    // Iterate over all inliers
    for(RegionMap::iterator it = points.begin(); it != points.end(); ++it) {
        // Check if this point was already processed
        if(it->second != -1)
            continue;

        // Add point to the queue
        queue.push(it);

        // Using std::vector instead of QVector because QVector does not support emplace
        sceneObjects.emplace_back(mObjName, labelImg.size(), sceneMgr);

        // Iterate until the whole region is marked
        while(!queue.empty()) {
            RegionMap::iterator current = queue.front();
            queue.pop();

            if(current->second != -1)
                continue;

            // This point is unmarked, so mark it with the current region ID
            current->second = currentId;

            const cv::Point& pixel = current->first;
            sceneObjects[currentId].addPoint(pixel);

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

    return sceneObjects;
}

bool Object::applyQualifier(const Qualifier& qual, std::vector<SceneObject>& objList) const {
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
