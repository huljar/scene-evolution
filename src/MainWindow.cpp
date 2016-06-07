#include <scene-evolution/MainWindow.h>
#include "ui_MainWindow.h"
#include <scene-evolution/util.h>

#include <QFileDialog>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mOgreWindow(new CustomOgreWindow)
    , mDatasetManager(nullptr)
    , mCurrentSceneIdx(0)
    , mRGBDScene(nullptr)
    , mRGBDSceneNode(nullptr)
{
    ui->setupUi(this);

    // Register NYU dataset
    mDatasetManager = requestNYUDir();
    if(!mDatasetManager) {
        std::cout << "Quitting ..." << std::endl;
        exit(1); // Not using QApplication::exit() since the event loop has not started yet
    }

    // Set up signal/slot connections
    setUpConnections();

    // Load OGRE context
    QWidget* ogreContainer = QWidget::createWindowContainer(mOgreWindow);
    ui->widgetOgreContainer->layout()->addWidget(ogreContainer);

    // Load first scene
    changeScene(mDatasetManager->loadScene(mDatasetManager->getAllSceneNames().front()), 0);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeScene(const Scene& scene, unsigned int sceneIdx) {
    // Emit pre scene change signal
    SceneChangingEventArgs preArgs(mCurrentScene.getFileName(), mCurrentSceneIdx, scene.getFileName(), sceneIdx);
    emit sceneChanging(preArgs);

    // Check if the scene change was cancelled by a slot
    if(preArgs.abort)
        return;

    // Perform the scene change
    mCurrentScene = scene;
    mCurrentSceneIdx = sceneIdx;

    delete mRGBDScene;
    mRGBDScene = new RGBDScene(mOgreWindow->getOgreSceneManager(), scene.getDepthImg(), scene.getRgbImg(), mCameraManager);

    // Emit post scene change signal
    SceneChangedEventArgs postArgs(scene.getFileName(), sceneIdx);
    emit sceneChanged(postArgs);
}

void MainWindow::onActionChangeNYUDirectoryTriggered(bool checked) {
    Q_UNUSED(checked);

    DatasetManager* tmp = requestNYUDir();
    if(tmp) {
        delete mDatasetManager;
        mDatasetManager = tmp;

        // Load first scene in dataset
        changeScene(mDatasetManager->loadScene(mDatasetManager->getAllSceneNames().front()), 0);
    }
}

void MainWindow::onActionExitTriggered(bool checked) {
    Q_UNUSED(checked);

    QApplication::quit();
}

void MainWindow::onPushButtonPrevSceneClicked(bool checked) {
    Q_UNUSED(checked);

    QStringList sceneNames = mDatasetManager->getAllSceneNames();

    if(mCurrentSceneIdx > 0)
        changeScene(mDatasetManager->loadScene(sceneNames[mCurrentSceneIdx - 1]), mCurrentSceneIdx - 1);
}

void MainWindow::onPushButtonNextSceneClicked(bool checked) {
    Q_UNUSED(checked);

    QStringList sceneNames = mDatasetManager->getAllSceneNames();

    if(mCurrentSceneIdx < static_cast<unsigned int>(sceneNames.size()) - 1)
        changeScene(mDatasetManager->loadScene(sceneNames[mCurrentSceneIdx + 1]), mCurrentSceneIdx + 1);
}

void MainWindow::onPushButtonGoToSceneClicked(bool checked) {
    Q_UNUSED(checked);

    unsigned int newIdx = static_cast<unsigned int>(ui->spinBoxGoToScene->value()); // min value is 0, so no underflow occurring
    QStringList sceneNames = mDatasetManager->getAllSceneNames();

    if(newIdx < static_cast<unsigned int>(sceneNames.size()))
        changeScene(mDatasetManager->loadScene(sceneNames[newIdx]), newIdx);
}

void MainWindow::onSceneChanging(SceneChangingEventArgs& e) {
    Q_UNUSED(e);
}

void MainWindow::onSceneChanged(SceneChangedEventArgs& e) {
    Q_UNUSED(e);

    // Adjust window title
    setWindowTitle(buildWindowTitle());

    // Enable/disable navigation buttons
    unsigned int sceneCount = mDatasetManager->getSceneCount();
    ui->pushButtonPrevScene->setEnabled(e.sceneIdx > 0);
    ui->pushButtonNextScene->setEnabled(e.sceneIdx < sceneCount - 1);

    // Set scene number in spin box (and max value in case the dataset changed)
    ui->spinBoxGoToScene->setMaximum(sceneCount - 1);
    ui->spinBoxGoToScene->setValue(e.sceneIdx);
}

DatasetManager* MainWindow::requestNYUDir() {
    // Request new dataset path until the user specifies a valid directory
    while(true) {
        QString nyuDirPath = QFileDialog::getExistingDirectory(this, tr("Select NYU Depth Dataset directory"), QDir::homePath(),
                                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        // Check if user canceled
        if(nyuDirPath == "")
            return nullptr;

        DatasetManager* ret = new DatasetManager(nyuDirPath); // Create dataset manager

        if(ret->checkIntegrity()) {
            std::cout << "Dataset directory \"" << nyuDirPath.toStdString() << "\" is valid." << std::endl;
            return ret;
        }

        std::cerr << "Error: dataset directory \"" << nyuDirPath.toStdString() << "\" does not contain an NYU Depth Dataset!" << std::endl;
        delete ret;
    }
}

void MainWindow::setUpConnections() {
    connect(ui->actionChangeNYUdirectory, SIGNAL(triggered(bool)), this, SLOT(onActionChangeNYUDirectoryTriggered(bool)));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExitTriggered(bool)));

    connect(ui->pushButtonPrevScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonPrevSceneClicked(bool)));
    connect(ui->pushButtonNextScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonNextSceneClicked(bool)));
    connect(ui->pushButtonGoToScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonGoToSceneClicked(bool)));

    connect(this, SIGNAL(sceneChanging(SceneChangingEventArgs&)), this, SLOT(onSceneChanging(SceneChangingEventArgs&)));
    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), this, SLOT(onSceneChanged(SceneChangedEventArgs&)));
}
