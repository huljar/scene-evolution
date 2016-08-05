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
QVariant DistanceTerm::calc(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    // Get object(s) to which the distance will be calculated
    std::vector<SceneObject> targets = mObj->getSceneObjects(rgbdScene, currentScene, labels);

    float shortestDist = std::numeric_limits<float>::infinity();

    // Precalculate 3D points of evaluated object
    std::vector<cv::Vec3f> evalPoints;
    cv::Mat_<unsigned char> evalPixels = obj.getPixels();
    const CameraManager& camMgr = rgbdScene->cameraManager();
    cv::Mat depthImg = currentScene.getDepthImg();

    for(cv::Mat_<unsigned char>::iterator it = evalPixels.begin(); it != evalPixels.end(); ++it) {
        if(*it == 255) {
            evalPoints.push_back(camMgr.getWorldForDepth(it.pos(), depthImg.at<unsigned short>(it.pos())));
        }
    }

    // Iterate over target objects
    for(std::vector<SceneObject>::iterator it = targets.begin(); it != targets.end(); ++it) {
        // Iterate over target object points
        cv::Mat_<unsigned char> targetPixels = it->getPixels();

        for(cv::Mat_<unsigned char>::iterator jt = targetPixels.begin(); jt != targetPixels.end(); ++jt) {
            if(*jt == 255) {
                // Calculate 3D point
                cv::Vec3f targetPoint = camMgr.getWorldForDepth(jt.pos(), depthImg.at<unsigned short>(jt.pos()));

                // Iterate over evaluated object points
                for(std::vector<cv::Vec3f>::iterator kt = evalPoints.begin(); kt != evalPoints.end(); ++kt) {
                    float currentDist = dist(*kt, targetPoint);
                    if(currentDist < shortestDist) shortestDist = currentDist;
                }
            }
        }
    }

    return QVariant(shortestDist);
}

DistanceTerm* DistanceTerm::clone() const {
    return new DistanceTerm(*this);
}

void DistanceTerm::print(std::ostream& os) const {
    os << "DistanceTerm";
}
