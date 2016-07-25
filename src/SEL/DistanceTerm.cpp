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

QVariant DistanceTerm::calc(RGBDScene* rgbdScene, const Scene& currentScene, const SceneObject& obj) const {
    // Iterate over label image
    const cv::Mat& labelImg = currentScene.getLabelImg();
    float shortest = std::numeric_limits<float>::infinity();

    for(cv::Mat_<unsigned short>::const_iterator it = labelImg.begin<unsigned short>(); it != labelImg.end<unsigned short>(); ++it) {

    }
}

DistanceTerm* DistanceTerm::clone() const {
    return new DistanceTerm(*this);
}

void DistanceTerm::print(std::ostream& os) const {
    os << "DistanceTerm";
}
