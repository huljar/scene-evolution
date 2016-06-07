#include <scene-evolution/CustomOgreWindow.h>

CustomOgreWindow::CustomOgreWindow(QWindow *parent)
    : QTOgreWindow(parent)
{
}

CustomOgreWindow::~CustomOgreWindow() {

}

void CustomOgreWindow::createCamera() {
    mCamera = mSceneMgr->createCamera("MainCamera");
    //mCamera->setPosition(Constants::InitialCameraPosition);
    //mCamera->lookAt(Constants::InitialCameraLookAt);
    //mCamera->setNearClipDistance(Constants::CameraNearClipDistance);
    //mCamera->setFOVy(Ogre::Degree(55));

    mCameraMan = new OgreQtBites::SdkQtCameraMan(mCamera);   // create a default camera controller
    //mCameraMan->setTopSpeed(15);
}

void CustomOgreWindow::createScene() {

}
