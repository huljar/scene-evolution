#include <SEL/DistanceTerm.h>
#include <limits>

using namespace SEL;

DistanceTerm::DistanceTerm(Object* obj)
    : mObj(obj)
{
}

DistanceTerm::DistanceTerm(const DistanceTerm& other)
    : mObj(other.mObj)
{
}

DistanceTerm& DistanceTerm::operator=(const DistanceTerm& other) {
    delete mObj;

    mObj = other.mObj;

    return *this;
}

DistanceTerm::~DistanceTerm() {
    delete mObj;
    std::cerr << "Deleting DistanceTerm" << std::endl;
}

// TODO: more efficient calculations (currently O(n*m))
QVariant DistanceTerm::calc(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    std::cout << "Calculating shortest distance..." << std::endl
              << "Term object: " << mObj->getName().toStdString() << std::endl
              << "Query object: " << obj.getName().toStdString() << std::endl;

    // Get object(s) to which the distance will be calculated
    std::vector<std::shared_ptr<SceneObject>> termObjects = mObj->getSceneObjects(sceneObjMgr, currentScene, labels);

    std::cout << "Term object exists " << termObjects.size() << " times in the scene" << std::endl;

    float shortestDist = std::numeric_limits<float>::infinity();

    // Precalculate 3D points of evaluated object
    std::vector<cv::Vec3f> queryPoints;
    cv::Mat_<unsigned char> queryPixels = obj.getPixels();
    const CameraManager& camMgr = sceneObjMgr->getRGBDScene()->cameraManager();
    cv::Mat depthImg = currentScene.getDepthImg();

    for(cv::Mat_<unsigned char>::iterator it = queryPixels.begin(); it != queryPixels.end(); ++it) {
        if(*it == 255) {
            queryPoints.push_back(camMgr.getWorldForDepth(it.pos(), depthImg.at<unsigned short>(it.pos())));
        }
    }

    // Iterate over target objects
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = termObjects.begin(); it != termObjects.end(); ++it) {
        // Iterate over target object points
        cv::Mat_<unsigned char> termObjPixels = (*it)->getPixels();

        for(cv::Mat_<unsigned char>::iterator jt = termObjPixels.begin(); jt != termObjPixels.end(); ++jt) {
            if(*jt == 255) {
                // Calculate 3D point
                cv::Vec3f termObjPoint = camMgr.getWorldForDepth(jt.pos(), depthImg.at<unsigned short>(jt.pos()));

                // Iterate over evaluated object points
                for(std::vector<cv::Vec3f>::iterator kt = queryPoints.begin(); kt != queryPoints.end(); ++kt) {
                    float currentDist = dist(*kt, termObjPoint);
                    if(currentDist < shortestDist) shortestDist = currentDist;
                }
            }
        }
    }

    std::cout << "Shortest distance found: " << shortestDist << std::endl;

    return QVariant(shortestDist);
}

DistanceTerm* DistanceTerm::clone() const {
    return new DistanceTerm(*this);
}

void DistanceTerm::print(std::ostream& os) const {
    os << "DistanceTerm";
}
