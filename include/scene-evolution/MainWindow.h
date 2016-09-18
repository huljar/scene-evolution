#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <scene-evolution/CustomOgreWindow.h>
#include <scene-evolution/DatasetManager.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/CameraManager.h>
#include <scene-evolution/EventArgs.h>
#include <scene-evolution/BoundingBoxManager.h>
#include <scene-evolution/SceneObjectManager.h>
#include <scene-evolution/LabelOverlayManager.h>
#include <scene-evolution/StatsManager.h>
#include <SEL/Driver.h>

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    bool changeDataset(DatasetManager* dataset);
    bool changeScene(const Scene& scene, unsigned int sceneIdx, const DatasetManager* datasetMgr = nullptr);

    void runSELQueries(const std::list<SEL::Query*>& queryList);

public slots:
    void onOgreInitialized();

    void onActionChangeNYUDirectoryTriggered(bool checked);
    void onActionExitTriggered(bool checked);

    void onActionLoadOBBsFromFileTriggered(bool checked);
    void onActionSaveOBBsToFileTriggered(bool checked);

    void onActionLoadSELFromFileTriggered(bool checked);
    void onActionRunSELTriggered(bool checked);

    void onPushButtonPrevSceneClicked(bool checked);
    void onPushButtonNextSceneClicked(bool checked);
    void onPushButtonGoToSceneClicked(bool checked);

    void onCheckBoxDisplayLabelsStateChanged(int state);
    void onCheckBoxDisplayLabelBordersStateChanged(int state);

    void onPushButtonStartNewBoxClicked(bool checked);
    void onPushButtonFinalizeBoxClicked(bool checked);
    void onPushButtonCancelBoxClicked(bool checked);

    void onPushButtonExecuteManualSELClicked(bool checked);

    void onCheckBoxSELBoundingBoxesStateChanged(int state);

    void onPushButtonComputeStatsClicked(bool checked);

    void onSceneChanged(SceneChangedEventArgs& e);
    void onDatasetChanged(DatasetChangedEventArgs& e);

signals:
    void datasetChanging(DatasetChangingEventArgs& e);
    void datasetChangingConfirmed(DatasetChangingConfirmedEventArgs& e);
    void datasetChanged(DatasetChangedEventArgs& e);

    void sceneChanging(SceneChangingEventArgs& e);
    void sceneChanged(SceneChangedEventArgs& e);

    void windowClosing(WindowClosingEventArgs& e);

protected:
    void closeEvent(QCloseEvent* e);

private:
    DatasetManager* requestNYUDir();
    void setUpConnections();
    void setUpSOMConnections();
    void setUpBBMConnections();
    void setUpLOMConnections();
    QString buildWindowTitle();
    void setBoundingBoxControlStates(bool enable);
    void restoreBoxDefaultsNoSignals();

    Ui::MainWindow* ui;

    CustomOgreWindow* mOgreWindow;
    DatasetManager* mDatasetManager;
    BoundingBoxManager* mBoundingBoxManager;
    LabelOverlayManager* mLabelOverlayManager;

    Scene mCurrentScene;
    unsigned int mCurrentSceneIdx;

    SceneObjectManager* mSceneObjectManager;

    SEL::Driver* mSELDriver;
    QString mLastSELDir;

    StatsManager* mStatsManager;
};

#endif // MAINWINDOW_H
