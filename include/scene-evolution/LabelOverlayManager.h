#ifndef LABELOVERLAYMANAGER_H
#define LABELOVERLAYMANAGER_H

#include <scene-evolution/EventArgs.h>
#include <scene-evolution/Scene.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QVector>

#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreWindowEventUtilities.h>

#include <opencv2/core/core.hpp>

#include <map>
#include <tuple>
#include <set>

class LabelOverlayManager : public QObject, public Ogre::WindowEventListener
{
    Q_OBJECT

public:
    class CameraLocker {
    public:
        CameraLocker(Ogre::Camera* camera);
        ~CameraLocker();
    private:
        Ogre::Camera* mCamera;
        Ogre::Vector3 mOrigPos;
        Ogre::Quaternion mOrigOrient;
    };

    // sceneIdx -> (overlay, minPx, fontsize, winWidth, winHeight) (on creation values)
    typedef QMap<unsigned int, std::tuple<Ogre::Overlay*, unsigned int, unsigned int, unsigned int, unsigned int>> SceneOverlayMap;
    // sceneIdx -> (overlay, material, texture of material, winWidth, winHeight) (on creation values)
    typedef QMap<unsigned int, std::tuple<Ogre::Overlay*, Ogre::MaterialPtr, Ogre::TexturePtr, unsigned int, unsigned int>> SceneBorderMap;

    typedef std::map<cv::Point, int, bool(*)(const cv::Point&, const cv::Point&)> RegionMap;
    typedef std::map<int, std::set<cv::Point, bool(*)(const cv::Point&, const cv::Point&)>> ClusterMap;

    typedef QMap<unsigned int, std::pair<RegionMap, ClusterMap>> SceneMapsMap;

    LabelOverlayManager(const Scene& currentScene, unsigned int currentSceneIdx, RGBDScene* currentRGBDScene,
                        const QVector<QString>& labels, unsigned int minLabelPx, unsigned int fontSize,
                        Ogre::Camera* ogreCamera, unsigned int windowWidth, unsigned int windowHeight);
    virtual ~LabelOverlayManager();

    LabelOverlayManager(const LabelOverlayManager& other) = delete;
    LabelOverlayManager& operator=(const LabelOverlayManager& other) = delete;

    // Ogre::WindowEventListener methods
    virtual void windowResized(Ogre::RenderWindow* rw);

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

    Ogre::Camera* mCamera;

    QVector<QString> mLabels;

    SceneOverlayMap mSceneOverlayMap;
    SceneBorderMap mSceneBorderMap;
    SceneMapsMap mSceneMapsMap;

    bool mLabelsEnabled;
    unsigned int mMinPx;
    unsigned int mFontSize;

    bool mLabelBordersEnabled;

    unsigned int mWindowWidth;
    unsigned int mWindowHeight;
};

#endif // LABELOVERLAYMANAGER_H
