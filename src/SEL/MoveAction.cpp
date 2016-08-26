#include <SEL/MoveAction.h>
#include <scene-evolution/util.h>
#include <scene-evolution/interop.h>

#include <chrono>
#include <cmath>

using namespace SEL;
using namespace interop;

std::default_random_engine MoveAction::msRandomEngine(std::chrono::system_clock::now().time_since_epoch().count());

MoveAction::MoveAction(Object* toObj, SearchCondition* searchCond)
    : mObj(toObj)
    , mSearchCond(searchCond)
{
}

MoveAction::MoveAction(const MoveAction& other)
    : mObj(other.mObj)
    , mSearchCond(other.mSearchCond)
{
}

MoveAction& MoveAction::operator=(const MoveAction& other) {
    delete mObj;
    delete mSearchCond;

    mObj = other.mObj;
    mSearchCond = other.mSearchCond;

    return *this;
}

MoveAction::~MoveAction() {
    delete mObj;
    delete mSearchCond;
}

void MoveAction::exec(SceneObjectManager* sceneObjMgr, const Scene& currentScene, const DatasetManager::LabelMap& labels,
                      std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const {
    RGBDScene* rgbdScene = sceneObjMgr->getRGBDScene();

    // Find appropriate new locations
    std::vector<std::shared_ptr<SceneObject>> targets = (mSearchCond
                                                         ? mObj->getSceneObjects(*mSearchCond, sceneObjMgr, currentScene, labels)
                                                         : mObj->getSceneObjects(sceneObjMgr, currentScene, labels));

    if(targets.size() == 0) {
        std::cerr << "No valid move targets found!" << std::endl;
        return;
    }

    std::vector<Ogre::Vector3> validPositions;

    // Collect all valid target positions
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = targets.begin(); it != targets.end(); ++it) {
        // Get valid positions on the target
        // TODO: eliminate selection of inplausible positions (e.g. too far on the edge), consider the moving object's size
        std::vector<Ogre::Vector3> vertices = (*it)->getVertices(currentScene.getDepthImg(), rgbdScene->cameraManager());

        // TODO: how to eliminate impossible targets?
        std::copy(vertices.begin(), vertices.end(), std::back_inserter(validPositions));

//        // Iterate over pixels
//        for(int y = 1; y < pixels.rows - 1; ++y) {
//            for(int x = 1; x < pixels.cols - 1; ++x) {
//                // Check if the pixels and its neighborhood all belong to the object
//                bool valid = true;
//                for(int i = y - 1; i <= y + 1; ++i) {
//                    for(int j = x - 1; j <= x + 1; ++j) {
//                        if(pixels(i, j) == 0) {
//                            valid = false;
//                            break;
//                        }
//                    }
//                    if(!valid) break;
//                }

//                // Continue with next pixel if this one is not good
//                if(!valid) continue;

//                // Compute x and y derivations
//                float yDeriv = slope(pixels3D(y - 1, x), pixels3D(y + 1, x));
//                float xDeriv = slope(pixels3D(y, x - 1), pixels3D(y, x + 1));

//                // Check if the derivations are below the threshold
//                if(yDeriv <= Constants::MoveActionSlopeThreshold && xDeriv <= Constants::MoveActionSlopeThreshold) {
//                    // Add this point to the valid points
//                    validPositions.push_back(pixels3D(y, x));
//                }
//            }
//        }
    }

    // Check if there is at least one valid position
    if(validPositions.size() == 0) {
        std::cerr << "No valid target positions found!" << std::endl;
        return;
    }

    // Cut and meshify the objects if necessary
    for(auto&& obj : selectedObjects) {
        if(!obj->hasManualObject()) {
            sceneObjMgr->cutObject(obj);
            obj->meshify(currentScene.getDepthImg(), currentScene.getRgbImg(), rgbdScene->cameraManager());
        }
    }

    // Execute move action for each selected object
    for(auto&& obj : selectedObjects) {
        // Select a random valid position
        Ogre::Vector3 targetPosition = validPositions[std::uniform_int_distribution<size_t>(0, validPositions.size() - 1)(msRandomEngine)];

        // Get 3D bounding box
        Ogre::AxisAlignedBox bb = obj->getBoundingBox(currentScene.getDepthImg(), rgbdScene->cameraManager());

        // Retrieve object height
        float height = bb.getHalfSize().y;

        // Compute new position
        targetPosition.y += height;

        std::cout << "Computed translation of " << targetPosition << " for object " << obj->getName().toStdString() << std::endl;

        // Set this translation on the object
        obj->setCurrentTranslation(ogreToCv(targetPosition));
    }
}

MoveAction* MoveAction::clone() const {
    return new MoveAction(*this);
}

float MoveAction::slope(const cv::Vec3f& a, const cv::Vec3f& b) const {
    float xzDist = std::sqrt(std::pow(a[0] - b[0], 2) + std::pow(a[2] - b[2], 2));
    float yDist = std::fabs(a[1] - b[1]);

    return yDist / xzDist;
}

void MoveAction::print(std::ostream& os) const {
    os << "MoveAction";
}
