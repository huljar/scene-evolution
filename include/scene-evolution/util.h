#ifndef UTIL_H
#define UTIL_H

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreVector3.h>

#include <QString>
#include <QStringList>

namespace Constants {
    const float WorkPlaneDepth = 5.0f;

    const Ogre::Vector3 InitialCameraPosition(0.0, 0.0, 0.0);
    const Ogre::Vector3 InitialCameraLookAt(0.0, 0.0, -1.0);
    const float CameraNearClipDistance = 5.0f;

    const Ogre::Vector3 InitialOBBCenter(0.0, 0.0, -500.0);
    const Ogre::Vector3 InitialOBBExtents(100.0, 100.0, 100.0);
    const Ogre::Vector3 InitialOBBEulerAngles(0.0, 0.0, 0.0);

    const QStringList OBBSupportedObjects = (QStringList() << "Book"
                                                           << "Cup"
                                                           << "Phone");
    const int InitialOBBObjectIndex = 0;

    const Ogre::ColourValue BoundingBoxActiveColour(1.0, 1.0, 0.0);
    const Ogre::ColourValue BoundingBoxInactiveColour = Ogre::ColourValue::White;
}

namespace Strings {
    const QString WindowTitleBase = "Scene Evolution";
    const QString TotalScenesLabelBase = " scenes total";

    const std::string StandardMaterialName = "BaseWhiteNoLighting";
    const std::string RGBDSceneNodeName = "rgbdScene";
    const std::string BoundingBoxMeshNameInactive = "obbMesh";
    const std::string BoundingBoxMeshNameActive = "obbMeshActive";
}

#endif // UTIL_H
