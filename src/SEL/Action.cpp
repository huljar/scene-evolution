#include <SEL/Action.h>

using namespace SEL;

Action::Action() {

}

Action::~Action() {

}

void Action::cutObject(SceneObject& obj, RGBDScene* rgbdScene) const {
    // Create mask from object outline and recreate scene mesh
    rgbdScene->meshify(obj.getPixels());
}

void Action::cutObjects(std::vector<SceneObject>& objs, RGBDScene* rgbdScene) const {
    cv::Mat_<unsigned char> mask = cv::Mat_<unsigned char>::zeros(rgbdScene->getDepthImage().size());

    // Build mask from all objects
    for(std::vector<SceneObject>::iterator it = objs.begin(); it != objs.end(); ++it) {
        cv::Mat_<unsigned char> pixels = it->getPixels();
        if(pixels.rows == mask.rows && pixels.cols == mask.cols) {
            for(cv::Mat_<unsigned char>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
                if(*it == 255) mask(it.pos()) = 255;
            }
        }
    }

    // Recreate scene mesh
    rgbdScene->meshify(mask);
}
