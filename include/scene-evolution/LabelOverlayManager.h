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
    typedef QMap<unsigned int, std::tuple<Ogre::Overlay*, unsigned int, unsigned int>> SceneOverlayMap;

    typedef std::map<cv::Point, int, bool(*)(const cv::Point&, const cv::Point&)> RegionMap;
    typedef std::map<int, std::set<cv::Point, bool(*)(const cv::Point&, const cv::Point&)>> ClusterMap;

    typedef QMap<unsigned int, std::pair<RegionMap, ClusterMap>> SceneMapsMap;

    LabelOverlayManager(const Scene& currentScene, unsigned int currentSceneIdx, RGBDScene* currentRGBDScene,
                        const QVector<QString>& labels, unsigned int minLabelPx, unsigned int fontSize);

public slots:
    void onDatasetChanged(DatasetChangedEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

    void onCheckBoxDisplayLabelsStateChanged(int state);
    void onSpinBoxMinLabelPxValueChanged(int value);
    void onHorizontalSliderLabelFontSizeValueChanged(int value);

private:
    void updateLabels(bool forceRecreate = false);

    ClusterMap doRegionGrowing(RegionMap& points) const;

    unsigned int mCurrentSceneIdx;
    Scene mCurrentScene;
    RGBDScene* mCurrentRGBDScene;

    QVector<QString> mLabels;

    SceneOverlayMap mSceneOverlayMap;
    SceneMapsMap mSceneMapsMap;

    bool mLabelsEnabled;
    unsigned int mMinPx;
    unsigned int mFontSize;
};

#endif // LABELOVERLAYMANAGER_H
