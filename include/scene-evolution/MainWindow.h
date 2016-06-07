#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <scene-evolution/CustomOgreWindow.h>
#include <scene-evolution/DatasetManager.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/RGBDScene.h>
#include <scene-evolution/CameraManager.h>
#include <scene-evolution/EventArgs.h>

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

    void changeScene(const Scene& scene, unsigned int sceneIdx);

public slots:
    void onActionChangeNYUDirectoryTriggered(bool checked);
    void onActionExitTriggered(bool checked);

    void onPushButtonPrevSceneClicked(bool checked);
    void onPushButtonNextSceneClicked(bool checked);
    void onPushButtonGoToSceneClicked(bool checked);

    void onSceneChanging(SceneChangingEventArgs& e);
    void onSceneChanged(SceneChangedEventArgs& e);

signals:
    void sceneChanging(SceneChangingEventArgs& e);
    void sceneChanged(SceneChangedEventArgs& e);

private:
    DatasetManager* requestNYUDir();
    void setUpConnections();

    Ui::MainWindow* ui;

    CustomOgreWindow* mOgreWindow;
    DatasetManager* mDatasetManager;

    Scene mCurrentScene;
    unsigned int mCurrentSceneIdx;

    RGBDScene* mRGBDScene;
    Ogre::SceneNode* mRGBDSceneNode;
    CameraManager* mCameraManager;
};

#endif // MAINWINDOW_H
