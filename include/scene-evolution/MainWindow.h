#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <scene-evolution/CustomOgreWindow.h>
#include <scene-evolution/DatasetManager.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/CameraManager.h>
#include <scene-evolution/EventArgs.h>
#include <scene-evolution/BoundingBoxManager.h>
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
    bool changeScene(const Scene& scene, unsigned int sceneIdx);

public slots:
    void onOgreInitialized();

    void onActionChangeNYUDirectoryTriggered(bool checked);
    void onActionExitTriggered(bool checked);

    void onActionLoadOBBsFromFileTriggered(bool checked);
    void onActionSaveOBBsToFileTriggered(bool checked);

    void onActionLoadSELFromFileTriggered(bool checked);

    void onPushButtonPrevSceneClicked(bool checked);
    void onPushButtonNextSceneClicked(bool checked);
    void onPushButtonGoToSceneClicked(bool checked);

    void onPushButtonStartNewBoxClicked(bool checked);
    void onPushButtonFinalizeBoxClicked(bool checked);
    void onPushButtonCancelBoxClicked(bool checked);

    void onDatasetChanging(DatasetChangingEventArgs& e);
    void onDatasetChanged(DatasetChangedEventArgs& e);

    void onSceneChanging(SceneChangingEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

signals:
    void datasetChanging(DatasetChangingEventArgs& e);
    void datasetChanged(DatasetChangedEventArgs& e);

    void sceneChanging(SceneChangingEventArgs& e);
    void sceneChanged(SceneChangedEventArgs& e);

    void windowClosing(WindowClosingEventArgs& e);

protected:
    void closeEvent(QCloseEvent* e);

private:
    DatasetManager* requestNYUDir();
    void setUpConnections();
    void setUpBBMConnections();
    QString buildWindowTitle();
    void setBoundingBoxControlStates(bool enable);
    void restoreBoxDefaultsNoSignals();

    Ui::MainWindow* ui;

    CustomOgreWindow* mOgreWindow;
    DatasetManager* mDatasetManager;
    BoundingBoxManager* mBoundingBoxManager;

    Scene mCurrentScene;
    unsigned int mCurrentSceneIdx;

    RGBDScene* mRGBDScene;
    Ogre::SceneNode* mRGBDSceneNode;

    SEL::Driver* mSELDriver;
};

#endif // MAINWINDOW_H
