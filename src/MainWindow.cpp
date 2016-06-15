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
    , mBoundingBoxManager(nullptr)
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

    // Do not load the first scene yet, this is done when OGRE is fully initialized

    // Fill bounding box object type selection menu
    ui->comboBoxBoundingBoxType->addItems(Constants::SupportedObjects);
}

MainWindow::~MainWindow() {
    delete mBoundingBoxManager;
    delete mDatasetManager;
    delete mOgreWindow;
    delete ui;
}

bool MainWindow::changeDataset(DatasetManager* dataset) {
    if(!dataset)
        throw std::invalid_argument("Cannot change dataset to a null pointer");

    // Emit pre dataset change signal
    DatasetChangingEventArgs preArgs(mDatasetManager->getDatasetDir().absolutePath(), dataset->getDatasetDir().absolutePath());
    emit datasetChanging(preArgs);

    // Check if the change shall be aborted
    if(preArgs.abort) {
        delete dataset;
        return false;
    }

    // Load first scene in dataset
    bool result = changeScene(dataset->loadScene(dataset->getAllSceneNames().front()), 0);
    if(!result)
        return false;

    // Perform the dataset change
    if(mDatasetManager != dataset) {
        delete mDatasetManager;
        mDatasetManager = dataset;
    }

    // Emit post dataset change signal
    DatasetChangedEventArgs postArgs(mDatasetManager->getDatasetDir().absolutePath());
    emit datasetChanged(postArgs);

    return true;
}

bool MainWindow::changeScene(const Scene& scene, unsigned int sceneIdx) {
    // Emit pre scene change signal
    SceneChangingEventArgs preArgs(mCurrentScene.getFileName(), mCurrentSceneIdx, scene.getFileName(), sceneIdx);
    emit sceneChanging(preArgs);

    // Check if the scene change was cancelled by a slot
    if(preArgs.abort)
        return false;

    // Perform the scene change
    mCurrentScene = scene;
    mCurrentSceneIdx = sceneIdx;

    // Clean up old scene
    delete mRGBDScene;

    // Create and attach new scene
    if(!mRGBDSceneNode)
        mRGBDSceneNode = mOgreWindow->getOgreSceneManager()->getRootSceneNode()->createChildSceneNode(Strings::RGBDSceneNodeName);

    mRGBDScene = new RGBDScene(mOgreWindow->getOgreSceneManager(), scene.getDepthImg(), scene.getRgbImg(), mDatasetManager->getCameraParams());
    mRGBDSceneNode->attachObject(mRGBDScene->getManualObject());

    // Emit post scene change signal
    SceneChangedEventArgs postArgs(scene.getFileName(), sceneIdx);
    emit sceneChanged(postArgs);

    return true;
}

void MainWindow::onOgreInitialized() {
    // Emit dataset "change" for the initially loaded dataset to trigger necessary setup code
    changeDataset(mDatasetManager);
}

void MainWindow::onActionChangeNYUDirectoryTriggered(bool checked) {
    Q_UNUSED(checked);

    DatasetManager* tmp = requestNYUDir();
    if(tmp) {
        changeDataset(tmp);
    }
}

void MainWindow::onActionExitTriggered(bool checked) {
    Q_UNUSED(checked);

    WindowClosingEventArgs args;
    emit windowClosing(args);

    if(!args.abort)
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

void MainWindow::onPushButtonStartNewBoxClicked(bool checked) {
    Q_UNUSED(checked);

    // Enable/disable controls
    QList<QDoubleSpinBox*> spinboxes = ui->groupBoxBoundingBoxes->findChildren<QDoubleSpinBox*>();
    QList<QComboBox*> comboboxes = ui->groupBoxBoundingBoxes->findChildren<QComboBox*>();

    for(QList<QDoubleSpinBox*>::iterator it = spinboxes.begin(); it != spinboxes.end(); ++it)
        (*it)->setEnabled(true);
    for(QList<QComboBox*>::iterator it = comboboxes.begin(); it != comboboxes.end(); ++it)
        (*it)->setEnabled(true);
    ui->pushButtonFinalizeBox->setEnabled(true);
    ui->pushButtonStartNewBox->setEnabled(false);

    // Check if bounding box manager exists
    if(!mBoundingBoxManager) {
        mBoundingBoxManager = new BoundingBoxManager(mOgreWindow->getOgreSceneManager(), mCurrentSceneIdx);
        setUpBBMConnections();
    }

    // Create new bounding box
}

void MainWindow::onDatasetChanging(DatasetChangingEventArgs& e) {
    Q_UNUSED(e);
}

void MainWindow::onDatasetChanged(DatasetChangedEventArgs& e) {
    Q_UNUSED(e);

    unsigned int sceneCount = mDatasetManager->getSceneCount();

    // Set max value of scene selection spin box
    ui->spinBoxGoToScene->setMaximum(sceneCount - 1);

    // Set total scenes label text
    ui->labelTotalScenes->setText(QString::number(sceneCount) + Strings::TotalScenesLabelBase);
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

    // Set scene number in spin box
    ui->spinBoxGoToScene->setValue(e.sceneIdx);
}

void MainWindow::closeEvent(QCloseEvent* e) {
    WindowClosingEventArgs args;
    emit windowClosing(args);

    if(args.abort)
        e->ignore();
    else
        e->accept();
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
    connect(mOgreWindow, SIGNAL(initialized()), this, SLOT(onOgreInitialized()));

    connect(ui->actionChangeNYUdirectory, SIGNAL(triggered(bool)), this, SLOT(onActionChangeNYUDirectoryTriggered(bool)));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExitTriggered(bool)));

    connect(ui->pushButtonPrevScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonPrevSceneClicked(bool)));
    connect(ui->pushButtonNextScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonNextSceneClicked(bool)));
    connect(ui->pushButtonGoToScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonGoToSceneClicked(bool)));

    connect(ui->pushButtonStartNewBox, SIGNAL(clicked(bool)), this, SLOT(onPushButtonStartNewBoxClicked(bool)));

    connect(this, SIGNAL(datasetChanging(DatasetChangingEventArgs&)), this, SLOT(onDatasetChanging(DatasetChangingEventArgs&)));
    connect(this, SIGNAL(datasetChanged(DatasetChangedEventArgs&)), this, SLOT(onDatasetChanged(DatasetChangedEventArgs&)));

    connect(this, SIGNAL(sceneChanging(SceneChangingEventArgs&)), this, SLOT(onSceneChanging(SceneChangingEventArgs&)));
    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), this, SLOT(onSceneChanged(SceneChangedEventArgs&)));
}

void MainWindow::setUpBBMConnections() {
    connect(this, SIGNAL(sceneChanging(SceneChangingEventArgs&)), mBoundingBoxManager, SLOT(onSceneChanging(SceneChangingEventArgs&)));
    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), mBoundingBoxManager, SLOT(onSceneChanged(SceneChangedEventArgs&)));

    connect(this, SIGNAL(windowClosing(WindowClosingEventArgs&)), mBoundingBoxManager, SLOT(onMainWindowClosing(WindowClosingEventArgs&)));
}

QString MainWindow::buildWindowTitle() {
    QString title = Strings::WindowTitleBase + " - " + mDatasetManager->getDatasetDir().dirName();
    if(!mCurrentScene.getFileName().isEmpty())
        title += " - " + mCurrentScene.getFileName();
    return title;
}
