#include <SEL/DistanceTerm.h>
#include <scene-evolution/util.h>

#include <algorithm>
#include <chrono>
#include <limits>

using namespace SEL;

std::default_random_engine DistanceTerm::msRandomEngine(std::chrono::system_clock::now().time_since_epoch().count());

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
}

QVariant DistanceTerm::calc(SceneObjectManager* sceneObjMgr, int sceneIdx, SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    std::cout << "Calculating shortest distance..." << std::endl
              << "Term object: " << mObj->getName().toStdString() << std::endl
              << "Query object: " << obj.getName().toStdString() << std::endl;

    // Get object(s) to which the distance will be calculated
    std::vector<std::shared_ptr<SceneObject>> termObjects = mObj->getSceneObjects(sceneObjMgr, sceneIdx, labels);

    std::cout << "Term object exists " << termObjects.size() << " times in the scene" << std::endl;

    float shortestSqrDist = std::numeric_limits<float>::infinity();
    const CameraManager& camMgr = (sceneIdx >= 0 ? sceneObjMgr->getRGBDScene(static_cast<unsigned int>(sceneIdx)) : sceneObjMgr->getRGBDScene())->cameraManager();
    cv::Mat depthImg = (sceneIdx >= 0 ? sceneObjMgr->getScene(static_cast<unsigned int>(sceneIdx)) : sceneObjMgr->getScene()).getDepthImg();

    // Iterate over target objects
    for(auto&& objPtr : termObjects) {
        // Get vertices of term object
        std::vector<Ogre::Vector3> termPoints = objPtr->getVertices(depthImg, camMgr);

        // Shuffle the vertices (for random selection)
        std::shuffle(termPoints.begin(), termPoints.end(), msRandomEngine);

        // Iterate over target object vertices
        std::vector<Ogre::Vector3>::const_iterator pEnd = (termPoints.size() > Constants::DistanceTermApproximationNumPoints
                                                           ? termPoints.begin() + Constants::DistanceTermApproximationNumPoints
                                                           : termPoints.end());

        for(std::vector<Ogre::Vector3>::const_iterator it = termPoints.cbegin(); it != pEnd; ++it) {
            // Execute nearest neighbor query from this point to the evaluated object
            std::vector<SceneObject::Neighbor> result = obj.findKNearestNeighbors(*it, 1, depthImg, camMgr);

            // Check the result
            if(!result.empty() && result.front().second < shortestSqrDist) {
                shortestSqrDist = result.front().second;
            }
        }
    }

    float shortestDist = std::sqrt(shortestSqrDist);

    std::cout << "Shortest distance found: " << shortestDist << std::endl;

    return QVariant(shortestDist);
}

DistanceTerm* DistanceTerm::clone() const {
    return new DistanceTerm(*this);
}

void DistanceTerm::print(std::ostream& os) const {
    os << "DistanceTerm";
}
