#include <SEL/BooleanTerm.h>

using namespace SEL;

BooleanTerm::BooleanTerm(BooleanTerm* left, BooleanFactor* right)
    : mLeft(left)
    , mRight(right)
{
}

BooleanTerm::BooleanTerm(const BooleanTerm& other)
    : mLeft(other.mLeft)
    , mRight(other.mRight)
{
}

BooleanTerm& BooleanTerm::operator=(const BooleanTerm& other) {
    delete mLeft;
    delete mRight;

    mLeft = other.mLeft;
    mRight = other.mRight;

    return *this;
}

BooleanTerm::~BooleanTerm() {
    delete mLeft;
    delete mRight;
}

bool BooleanTerm::eval(SceneObjectManager* sceneObjMgr, int sceneIdx, SceneObject& obj, const DatasetManager::LabelMap& labels) const {
    if(mLeft)
        return mLeft->eval(sceneObjMgr, sceneIdx, obj, labels) && mRight->eval(sceneObjMgr, sceneIdx, obj, labels);

    return mRight->eval(sceneObjMgr, sceneIdx, obj, labels);
}

BooleanTerm* BooleanTerm::clone() const {
    return new BooleanTerm(*this);
}

void BooleanTerm::print(std::ostream& os) const {
    os << "BooleanTerm";
}
