#include <SEL/CopyAction.h>

#include <chrono>

using namespace SEL;
using namespace interop;

std::default_random_engine CopyAction::msRandomEngine(std::chrono::system_clock::now().time_since_epoch().count());

CopyAction::CopyAction(Object* toObj, SearchCondition* searchCond)
    : mObj(toObj)
    , mSearchCond(searchCond)
{
}

CopyAction::CopyAction(const CopyAction& other)
    : mObj(other.mObj)
    , mSearchCond(other.mSearchCond)
{
}

CopyAction& CopyAction::operator=(const CopyAction& other) {
    delete mObj;
    delete mSearchCond;

    mObj = other.mObj;
    mSearchCond = other.mSearchCond;

    return *this;
}

CopyAction::~CopyAction() {
    delete mObj;
    delete mSearchCond;
}

void CopyAction::exec(SceneObjectManager* sceneObjMgr, const DatasetManager::LabelMap& labels, std::vector<std::shared_ptr<SceneObject>>& selectedObjects) const {
    Scene currentScene = sceneObjMgr->getScene();
    RGBDScene* rgbdScene = sceneObjMgr->getRGBDScene();

    // Find appropriate new locations
    std::vector<std::shared_ptr<SceneObject>> targets = (mSearchCond
                                                         ? mObj->getSceneObjects(*mSearchCond, sceneObjMgr, -1, labels)
                                                         : mObj->getSceneObjects(sceneObjMgr, -1, labels));

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
    }

    // Check if there is at least one valid position
    if(validPositions.size() == 0) {
        std::cerr << "No valid target positions found!" << std::endl;
        return;
    }

    // Execute copy action for each selected object
    for(auto&& obj : selectedObjects) {
        // Select a random valid position
        Ogre::Vector3 targetPosition = validPositions[std::uniform_int_distribution<size_t>(0, validPositions.size() - 1)(msRandomEngine)];

        // Get 3D bounding box
        Ogre::AxisAlignedBox bb = obj->getBoundingBox(currentScene.getDepthImg(), rgbdScene->cameraManager());

        // Retrieve object height
        float height = bb.getHalfSize().y;

        // Compute new position
        targetPosition.y += height;

        // Clone object
        std::shared_ptr<SceneObject> newObj(obj->clone());

        std::cout << "Computed position of " << targetPosition << " for new object " << newObj->getName().toStdString() << std::endl;

        // Set this translation on the new object
        newObj->setCurrentTranslation(ogreToCv(targetPosition));

        // Set correct scene index
        newObj->setSceneIdx(sceneObjMgr->getCurrentSceneIdx());

        // Register new object with scene object manager
        sceneObjMgr->registerObject(newObj);
    }
}

CopyAction* CopyAction::clone() const {
    return new CopyAction(*this);
}

void CopyAction::print(std::ostream& os) const {
    os << "CopyAction";
}
