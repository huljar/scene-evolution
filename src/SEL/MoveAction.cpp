#include <SEL/MoveAction.h>
#include <scene-evolution/util.h>

#include <chrono>
#include <cmath>

using namespace SEL;

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

    std::vector<cv::Vec3f> validPositions;

    // Collect all valid target positions
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = targets.begin(); it != targets.end(); ++it) {
        // Get valid positions on top of the target
        // TODO: eliminate selection of inplausible positions (e.g. too far on the edge), consider the moving object's size
        cv::Mat_<unsigned char> pixels = (*it)->getPixels();
        if(!(*it)->has3DPixels())
            (*it)->create3DPixels(currentScene.getDepthImg(), rgbdScene->cameraManager());
        cv::Mat_<cv::Vec3f> pixels3D = (*it)->get3DPixels();

        // Iterate over pixels
        for(int y = 1; y < pixels.rows - 1; ++y) {
            for(int x = 1; x < pixels.cols - 1; ++x) {
                // Check if the pixels and its neighborhood all belong to the object
                bool valid = true;
                for(int i = y - 1; i <= y + 1; ++i) {
                    for(int j = x - 1; j <= x + 1; ++j) {
                        if(pixels(i, j) == 0) {
                            valid = false;
                            break;
                        }
                    }
                    if(!valid) break;
                }

                // Continue with next pixel if this one is not good
                if(!valid) continue;

                // Compute x and y derivations
                float yDeriv = slope(pixels3D(y - 1, x), pixels3D(y + 1, x));
                float xDeriv = slope(pixels3D(y, x - 1), pixels3D(y, x + 1));

                // Check if the derivations are below the threshold
                if(yDeriv <= Constants::MoveActionSlopeThreshold && xDeriv <= Constants::MoveActionSlopeThreshold) {
                    // Add this point to the valid points
                    validPositions.push_back(pixels3D(y, x));
                }
            }
        }
    }

    // Check if there is at least one valid position
    if(validPositions.size() == 0) {
        std::cerr << "No valid target positions found!" << std::endl;
        return;
    }

    // Cut the objects
    sceneObjMgr->cutObjects(selectedObjects);

    // Execute move action for each selected object
    for(std::vector<std::shared_ptr<SceneObject>>::iterator it = selectedObjects.begin(); it != selectedObjects.end(); ++it) {
        // Build meshes for object
        if(!(*it)->hasManualObject())
            (*it)->meshify(currentScene.getDepthImg(), currentScene.getRgbImg(), rgbdScene->cameraManager());

        // Select a random valid position
        cv::Vec3f targetPosition = validPositions[std::uniform_int_distribution<size_t>(0, validPositions.size() - 1)(msRandomEngine)];

        // Get original 3D bounding box of current object
        SceneObject::BoundingBox3D bb = (*it)->getBoundingBox3D(currentScene.getDepthImg(), rgbdScene->cameraManager());

        // Compute original object center
        cv::Vec3f center(bb.first[0] + (bb.second[0] - bb.first[0]) / 2.f,
                         bb.first[1] + (bb.second[1] - bb.first[1]) / 2.f,
                         bb.first[2] + (bb.second[2] - bb.first[2]) / 2.f);

        // Compute translation vector
        targetPosition[1] += center[1] - bb.first[1];
        cv::Vec3f trans = targetPosition - center;

        std::cout << "Computed translation of " << trans << " for object " << (*it)->getName().toStdString() << std::endl;

        // Set this translation on the object (with respect to original position)
        (*it)->setCurrentTranslation(trans);
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
