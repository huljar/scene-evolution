#ifndef UTIL_H
#define UTIL_H

#include <OGRE/OgreVector3.h>

#include <QString>

namespace Constants {
    const float WorkPlaneDepth = 5.0f;

    const Ogre::Vector3 InitialCameraPosition(0.0, 0.0, 0.0);
    const Ogre::Vector3 InitialCameraLookAt(0.0, 0.0, -1.0);
    const float CameraNearClipDistance = 5.0f;
}

namespace Strings {
    const QString WindowTitleBase = "Scene Evolution";
    const std::string StandardMaterialName = "BaseWhiteNoLighting";
    const std::string RGBDSceneNodeName = "rgbdScene";
}

#endif // UTIL_H
