#ifndef LABELOVERLAYMANAGER_H
#define LABELOVERLAYMANAGER_H

#include <scene-evolution/EventArgs.h>
#include <scene-evolution/Scene.h>

#include <QObject>
#include <QMap>

#include <OGRE/Overlay/OgreOverlay.h>

class LabelOverlayManager : public QObject
{
    Q_OBJECT

public:
    typedef QMap<unsigned int, std::pair<Ogre::Overlay*, unsigned int>> SceneOverlayMap;

    LabelOverlayManager(const Scene& currentScene, unsigned int currentSceneIdx, int minLabelPx);

public slots:
    void onDatasetChanged(DatasetChangedEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

    void onCheckBoxDisplayLabelsStateChanged(int state);
    void onSpinBoxMinLabelPxValueChanged(int value);

private:
    void updateLabels(bool forceRecreate = false);

    unsigned int mCurrentSceneIdx;
    Scene mCurrentScene;

    SceneOverlayMap mSceneOverlayMap;

    bool mLabelsEnabled;
    unsigned int mMinPx;
};

#endif // LABELOVERLAYMANAGER_H
