#ifndef LABELOVERLAYMANAGER_H
#define LABELOVERLAYMANAGER_H

#include <scene-evolution/EventArgs.h>
#include <scene-evolution/Scene.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QVector>

#include <OGRE/Overlay/OgreOverlay.h>

#include <opencv2/core/core.hpp>

#include <map>
#include <tuple>
#include <set>

class LabelOverlayManager : public QObject
{
    Q_OBJECT

public:
    typedef QMap<unsigned int, std::tuple<Ogre::Overlay*, unsigned int, unsigned int>> SceneOverlayMap; // sceneIdx -> (overlay, minPx on creation, fontsize on creation)
    typedef QMap<unsigned int, std::tuple<Ogre::Overlay*, Ogre::MaterialPtr, Ogre::TexturePtr>> SceneBorderMap; // sceneIdx -> (overlay, material, texture of material)

    typedef std::map<cv::Point, int, bool(*)(const cv::Point&, const cv::Point&)> RegionMap;
    typedef std::map<int, std::set<cv::Point, bool(*)(const cv::Point&, const cv::Point&)>> ClusterMap;

    typedef QMap<unsigned int, std::pair<RegionMap, ClusterMap>> SceneMapsMap;

    LabelOverlayManager(const Scene& currentScene, unsigned int currentSceneIdx, RGBDScene* currentRGBDScene,
                        const QVector<QString>& labels, unsigned int minLabelPx, unsigned int fontSize);
    virtual ~LabelOverlayManager();

public slots:
    void onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

    void onCheckBoxDisplayLabelsStateChanged(int state);
    void onSpinBoxMinLabelPxValueChanged(int value);
    void onHorizontalSliderLabelFontSizeValueChanged(int value);

    void onCheckBoxDisplayLabelBordersStateChanged(int state);

private:
    void updateLabels(bool forceRecreate = false);
    void updateLabelBorders(bool forceRecreate = false);

    void createCurrentMaps();
    ClusterMap doRegionGrowing(RegionMap& points) const;

    unsigned int mCurrentSceneIdx;
    Scene mCurrentScene;
    RGBDScene* mCurrentRGBDScene;

    QVector<QString> mLabels;

    SceneOverlayMap mSceneOverlayMap;
    SceneBorderMap mSceneBorderMap;
    SceneMapsMap mSceneMapsMap;

    bool mLabelsEnabled;
    unsigned int mMinPx;
    unsigned int mFontSize;

    bool mLabelBordersEnabled;
};

#endif // LABELOVERLAYMANAGER_H
