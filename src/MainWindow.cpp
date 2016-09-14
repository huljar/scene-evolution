#include <scene-evolution/MainWindow.h>
#include "ui_MainWindow.h"
#include <scene-evolution/util.h>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QSignalBlocker>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mOgreWindow(new CustomOgreWindow)
    , mDatasetManager(nullptr)
    , mBoundingBoxManager(nullptr)
    , mLabelOverlayManager(nullptr)
    , mCurrentSceneIdx(0)
    , mSceneObjectManager(nullptr)
    , mSELDriver(nullptr)
    , mLastSELDir(QDir::homePath())
{
    ui->setupUi(this);

    // Register NYU dataset
    mDatasetManager = requestNYUDir();
    if(!mDatasetManager) {
        std::cout << "Quitting ..." << std::endl;
        exit(0); // Not using QApplication::exit() since the event loop has not started yet
    }

    // Set up signal/slot connections
    setUpConnections();

    // Load OGRE context
    QWidget* ogreContainer = QWidget::createWindowContainer(mOgreWindow);
    ui->widgetOgreContainer->layout()->addWidget(ogreContainer);

    // Do not load the first scene yet, this is done when OGRE is fully initialized

    // Set bounding box initial values in UI
    ui->doubleSpinBoxBoundingBoxCenterX->setValue(Constants::InitialOBBCenter.x);
    ui->doubleSpinBoxBoundingBoxCenterY->setValue(Constants::InitialOBBCenter.y);
    ui->doubleSpinBoxBoundingBoxCenterZ->setValue(Constants::InitialOBBCenter.z);
    ui->doubleSpinBoxBoundingBoxExtentX->setValue(Constants::InitialOBBExtents.x);
    ui->doubleSpinBoxBoundingBoxExtentY->setValue(Constants::InitialOBBExtents.y);
    ui->doubleSpinBoxBoundingBoxExtentZ->setValue(Constants::InitialOBBExtents.z);
    ui->doubleSpinBoxBoundingBoxAngleX->setValue(Constants::InitialOBBEulerAngles.x);
    ui->doubleSpinBoxBoundingBoxAngleY->setValue(Constants::InitialOBBEulerAngles.y);
    ui->doubleSpinBoxBoundingBoxAngleZ->setValue(Constants::InitialOBBEulerAngles.z);

    ui->comboBoxBoundingBoxType->addItems(Constants::OBBSupportedObjects);
    ui->comboBoxBoundingBoxType->setCurrentIndex(Constants::InitialOBBObjectIndex);
}

MainWindow::~MainWindow() {
    delete mSELDriver;
    delete mSceneObjectManager;
    delete mLabelOverlayManager;
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
    bool result = changeScene(dataset->loadScene(dataset->getAllSceneNames().front()), 0, dataset);
    if(!result)
        return false;

    // Perform the dataset change
    if(mDatasetManager != dataset) {
        delete mDatasetManager;
        mDatasetManager = dataset;
    }

    // Emit post dataset change signal
    DatasetChangedEventArgs postArgs(mDatasetManager->getDatasetDir().absolutePath(), mDatasetManager->getLabelNames());
    emit datasetChanged(postArgs);

    return true;
}

bool MainWindow::changeScene(const Scene& scene, unsigned int sceneIdx, const DatasetManager* datasetMgr) {
    // Emit pre scene change signal
    SceneChangingEventArgs preArgs(mCurrentScene, mCurrentSceneIdx, scene, sceneIdx);
    emit sceneChanging(preArgs);

    // Check if the scene change was cancelled by a slot
    if(preArgs.abort)
        return false;

    // Emit dataset change confirmed if this is a dataset change
    if(datasetMgr) {
        DatasetChangingConfirmedEventArgs confArgs(datasetMgr->getDatasetDir().absolutePath(), datasetMgr->getLabelNames());
        emit datasetChangingConfirmed(confArgs);
    }

    // Perform the scene change
    mCurrentScene = scene;
    mCurrentSceneIdx = sceneIdx;

    // Check if the scene object manager already exists and owns an RGBDScene object for the new scene
    RGBDScene* newRGBDScene;
    if(!mSceneObjectManager) {
        // Create the scene
        newRGBDScene = new RGBDScene(mOgreWindow->getOgreSceneManager(), scene.getDepthImg(), scene.getRgbImg(), (datasetMgr ? datasetMgr : mDatasetManager)->getCameraParams());

        // Create the scene object manager
        mSceneObjectManager = new SceneObjectManager(scene, sceneIdx, newRGBDScene, ui->checkBoxSELBoundingBoxes->checkState() != Qt::Unchecked);
        setUpSOMConnections();
    }
    else {
        newRGBDScene = mSceneObjectManager->getRGBDScene(mCurrentSceneIdx);
        if(!newRGBDScene) {
            // Create the scene
            newRGBDScene = new RGBDScene(mOgreWindow->getOgreSceneManager(), scene.getDepthImg(), scene.getRgbImg(), (datasetMgr ? datasetMgr : mDatasetManager)->getCameraParams());
        }
    }

    // Emit post scene change signal
    SceneChangedEventArgs postArgs(scene, sceneIdx, newRGBDScene);
    emit sceneChanged(postArgs);

    return true;
}

void MainWindow::runSELQueries(const std::list<SEL::Query*>& queryList) {
    for(auto&& queryPtr : queryList) {
        // Check if any scenes need to be preregistered with the scene object manager
        // Check if the fromScene index is valid
        int fromScene = queryPtr->getSelectStmt()->getFromSceneIdx();
        if(fromScene >= 0) { // index of -1 indicates that the current scene shall be used
            // Check if a scene with this index exists
            unsigned int fromSceneIdx = static_cast<unsigned int>(fromScene);
            if(fromSceneIdx >= mDatasetManager->getSceneCount()) {
                std::cerr << "Error: fromScene index " << fromSceneIdx << " is out of bounds! Skipping SEL query..." << std::endl;
                continue;
            }

            // Check if scene exists in the scene object manager
            if(!mSceneObjectManager->getRGBDScene(fromSceneIdx)) {
                // Create scene
                Scene newScene = mDatasetManager->loadScene(mDatasetManager->getAllSceneNames().at(fromScene)); // Using the non-cast idx because Qt expects an int
                RGBDScene* newRGBDScene = new RGBDScene(mOgreWindow->getOgreSceneManager(), newScene.getDepthImg(), newScene.getRgbImg(), mDatasetManager->getCameraParams());

                // Register scene
                if(!mSceneObjectManager->preregisterScene(SceneChangedEventArgs(newScene, fromSceneIdx, newRGBDScene)))
                    delete newRGBDScene;
            }
        }

        // Execute
        queryPtr->exec(mSceneObjectManager, mDatasetManager->getLabelMap());
    }
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

void MainWindow::onActionLoadOBBsFromFileTriggered(bool checked) {
    Q_UNUSED(checked);

    // Check if bounding box manager exists
    if(!mBoundingBoxManager) {
        mBoundingBoxManager = new BoundingBoxManager(mOgreWindow->getOgreSceneManager(), mCurrentSceneIdx, mDatasetManager->getDatasetDir().absolutePath());
        setUpBBMConnections();
        ui->actionSaveOBBsToFile->setEnabled(true);
    }

    mBoundingBoxManager->loadFromFile();
}

void MainWindow::onActionSaveOBBsToFileTriggered(bool checked) {
    Q_UNUSED(checked);

    mBoundingBoxManager->saveToFile();
}

void MainWindow::onActionLoadSELFromFileTriggered(bool checked) {
    Q_UNUSED(checked);

    QString path = QFileDialog::getOpenFileName(nullptr, tr("Open file"), mLastSELDir, tr("Scene Evolution Language scripts (*.sel)"));
    if(!path.isEmpty()) {
        mLastSELDir = QFileInfo(path).absolutePath();

        if(!mSELDriver)
            mSELDriver = new SEL::Driver;

        int res = mSELDriver->parse(path);
        switch(res) {
            case 0: std::cout << "Successfully parsed file " << path.toStdString() << std::endl; ui->actionRunSEL->setEnabled(true); break;
            case 1: std::cerr << "Error: Syntax error in file " << path.toStdString() << std::endl; break;
            case 2: std::cerr << "Error: Memory exhaustion while parsing file " << path.toStdString() << std::endl; break;
            default: std::cerr << "Error: Unknown error occurred while parsing file " << path.toStdString() << std::endl; break;
        }
    }
}

void MainWindow::onActionRunSELTriggered(bool checked) {
    Q_UNUSED(checked);

    if(!mSELDriver) {
        std::cerr << "Error: No SEL file was loaded yet" << std::endl;
        return;
    }

    std::list<SEL::Query*> queries = mSELDriver->getResult();
    std::cout << "Executing " << queries.size() << " queries" << std::endl;

    runSELQueries(queries);
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

void MainWindow::onCheckBoxDisplayLabelsStateChanged(int state) {
    // Check if label overlay manager exists
    if(!mLabelOverlayManager) {
        mLabelOverlayManager = new LabelOverlayManager(mCurrentScene,
                                                       mCurrentSceneIdx,
                                                       mSceneObjectManager->getRGBDScene(),
                                                       mDatasetManager->getLabelNames(),
                                                       static_cast<unsigned int>(std::max(1, ui->spinBoxMinLabelPx->value())),
                                                       static_cast<unsigned int>(std::max(1, ui->horizontalSliderLabelFontSize->value() * 5)));
        setUpLOMConnections();

        // "Emit" state change again so the label overlay manager sees it
        mLabelOverlayManager->onCheckBoxDisplayLabelsStateChanged(state);
    }
}

void MainWindow::onCheckBoxDisplayLabelBordersStateChanged(int state) {
    // Check if label overlay manager exists
    if(!mLabelOverlayManager) {
        mLabelOverlayManager = new LabelOverlayManager(mCurrentScene,
                                                       mCurrentSceneIdx,
                                                       mSceneObjectManager->getRGBDScene(),
                                                       mDatasetManager->getLabelNames(),
                                                       static_cast<unsigned int>(std::max(1, ui->spinBoxMinLabelPx->value())),
                                                       static_cast<unsigned int>(std::max(1, ui->horizontalSliderLabelFontSize->value() * 5)));
        setUpLOMConnections();

        // "Emit" state change again so the label overlay manager sees it
        mLabelOverlayManager->onCheckBoxDisplayLabelBordersStateChanged(state);
    }
}

void MainWindow::onPushButtonStartNewBoxClicked(bool checked) {
    Q_UNUSED(checked);

    // Enable/disable controls
    setBoundingBoxControlStates(true);

    // Check if bounding box manager exists
    if(!mBoundingBoxManager) {
        mBoundingBoxManager = new BoundingBoxManager(mOgreWindow->getOgreSceneManager(), mCurrentSceneIdx, mDatasetManager->getDatasetDir().absolutePath());
        setUpBBMConnections();
        ui->actionSaveOBBsToFile->setEnabled(true);

        // "Emit" button click again so the bounding box manager sees it
        mBoundingBoxManager->onPushButtonStartNewBoxClicked(false);
    }
}

void MainWindow::onPushButtonFinalizeBoxClicked(bool checked) {
    Q_UNUSED(checked);

    // Enable/disable controls
    setBoundingBoxControlStates(false);

    // Restore defaults
    restoreBoxDefaultsNoSignals();
}

void MainWindow::onPushButtonCancelBoxClicked(bool checked) {
    Q_UNUSED(checked);

    // Enable/disable controls
    setBoundingBoxControlStates(false);

    // Restore defaults
    restoreBoxDefaultsNoSignals();
}

void MainWindow::onPushButtonExecuteManualSELClicked(bool checked) {
    Q_UNUSED(checked);

    if(!mSELDriver)
        mSELDriver = new SEL::Driver;

    int res = mSELDriver->parseString(ui->textEditManualSEL->document()->toPlainText());
    switch(res) {
        case 0: std::cout << "Successfully parsed string" << std::endl; ui->actionRunSEL->setEnabled(false); break;
        case 1: std::cerr << "Error: Syntax error in string" << std::endl; return;
        case 2: std::cerr << "Error: Memory exhaustion while parsing file" << std::endl; return;
        default: std::cerr << "Error: Unknown error occurred while parsing file" << std::endl; return;
    }

    std::list<SEL::Query*> queries = mSELDriver->getResult();
    std::cout << "Executing " << queries.size() << " queries" << std::endl;

    runSELQueries(queries);
}

void MainWindow::onCheckBoxSELBoundingBoxesStateChanged(int state) {
    if(mSceneObjectManager)
        mSceneObjectManager->setShowBoundingBoxes(state != Qt::Unchecked);
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

    // Restore bounding box UI controls in case they are still active
    setBoundingBoxControlStates(false);
    restoreBoxDefaultsNoSignals();
}

void MainWindow::onDatasetChanged(DatasetChangedEventArgs& e) {
    Q_UNUSED(e);

    unsigned int sceneCount = mDatasetManager->getSceneCount();

    // Set max value of scene selection spin box
    ui->spinBoxGoToScene->setMaximum(sceneCount - 1);

    // Set total scenes label text
    ui->labelTotalScenes->setText(QString::number(sceneCount) + Strings::TotalScenesLabelBase);

    // Adjust window title
    setWindowTitle(buildWindowTitle());
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

    connect(ui->actionLoadOBBsFromFile, SIGNAL(triggered(bool)), this, SLOT(onActionLoadOBBsFromFileTriggered(bool)));
    connect(ui->actionSaveOBBsToFile, SIGNAL(triggered(bool)), this, SLOT(onActionSaveOBBsToFileTriggered(bool)));

    connect(ui->actionLoadSELFromFile, SIGNAL(triggered(bool)), this, SLOT(onActionLoadSELFromFileTriggered(bool)));
    connect(ui->actionRunSEL, SIGNAL(triggered(bool)), this, SLOT(onActionRunSELTriggered(bool)));

    connect(ui->pushButtonPrevScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonPrevSceneClicked(bool)));
    connect(ui->pushButtonNextScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonNextSceneClicked(bool)));
    connect(ui->pushButtonGoToScene, SIGNAL(clicked(bool)), this, SLOT(onPushButtonGoToSceneClicked(bool)));

    connect(ui->checkBoxDisplayLabels, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxDisplayLabelsStateChanged(int)));
    connect(ui->checkBoxDisplayLabelBorders, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxDisplayLabelBordersStateChanged(int)));

    connect(ui->pushButtonStartNewBox, SIGNAL(clicked(bool)), this, SLOT(onPushButtonStartNewBoxClicked(bool)));
    connect(ui->pushButtonFinalizeBox, SIGNAL(clicked(bool)), this, SLOT(onPushButtonFinalizeBoxClicked(bool)));
    connect(ui->pushButtonCancelBox, SIGNAL(clicked(bool)), this, SLOT(onPushButtonCancelBoxClicked(bool)));

    connect(ui->pushButtonExecuteManualSEL, SIGNAL(clicked(bool)), this, SLOT(onPushButtonExecuteManualSELClicked(bool)));
    connect(ui->checkBoxSELBoundingBoxes, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxSELBoundingBoxesStateChanged(int)));

    connect(this, SIGNAL(datasetChanged(DatasetChangedEventArgs&)), this, SLOT(onDatasetChanged(DatasetChangedEventArgs&)));

    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), this, SLOT(onSceneChanged(SceneChangedEventArgs&)));
}

void MainWindow::setUpSOMConnections() {
    connect(this, SIGNAL(datasetChangingConfirmed(DatasetChangingConfirmedEventArgs&)), mSceneObjectManager, SLOT(onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs&)));

    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), mSceneObjectManager, SLOT(onSceneChanged(SceneChangedEventArgs&)));
}

void MainWindow::setUpBBMConnections() {
    connect(this, SIGNAL(datasetChanging(DatasetChangingEventArgs&)), mBoundingBoxManager, SLOT(onDatasetChanging(DatasetChangingEventArgs&)));
    connect(this, SIGNAL(datasetChangingConfirmed(DatasetChangingConfirmedEventArgs&)), mBoundingBoxManager, SLOT(onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs&)));

    connect(this, SIGNAL(sceneChanging(SceneChangingEventArgs&)), mBoundingBoxManager, SLOT(onSceneChanging(SceneChangingEventArgs&)));
    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), mBoundingBoxManager, SLOT(onSceneChanged(SceneChangedEventArgs&)));

    connect(this, SIGNAL(windowClosing(WindowClosingEventArgs&)), mBoundingBoxManager, SLOT(onMainWindowClosing(WindowClosingEventArgs&)));

    connect(ui->pushButtonStartNewBox, SIGNAL(clicked(bool)), mBoundingBoxManager, SLOT(onPushButtonStartNewBoxClicked(bool)));

    connect(ui->doubleSpinBoxBoundingBoxCenterX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxCenterValueChanged(d, ui->doubleSpinBoxBoundingBoxCenterY->value(), ui->doubleSpinBoxBoundingBoxCenterZ->value());
    });
    connect(ui->doubleSpinBoxBoundingBoxCenterY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxCenterValueChanged(ui->doubleSpinBoxBoundingBoxCenterX->value(), d, ui->doubleSpinBoxBoundingBoxCenterZ->value());
    });
    connect(ui->doubleSpinBoxBoundingBoxCenterZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxCenterValueChanged(ui->doubleSpinBoxBoundingBoxCenterX->value(), ui->doubleSpinBoxBoundingBoxCenterY->value(), d);
    });

    connect(ui->doubleSpinBoxBoundingBoxExtentX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxExtentsValueChanged(d, ui->doubleSpinBoxBoundingBoxExtentY->value(), ui->doubleSpinBoxBoundingBoxExtentZ->value());
    });
    connect(ui->doubleSpinBoxBoundingBoxExtentY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxExtentsValueChanged(ui->doubleSpinBoxBoundingBoxExtentX->value(), d, ui->doubleSpinBoxBoundingBoxExtentZ->value());
    });
    connect(ui->doubleSpinBoxBoundingBoxExtentZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxExtentsValueChanged(ui->doubleSpinBoxBoundingBoxExtentX->value(), ui->doubleSpinBoxBoundingBoxExtentY->value(), d);
    });

    connect(ui->doubleSpinBoxBoundingBoxAngleX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxEulerAnglesValueChanged(d, ui->doubleSpinBoxBoundingBoxAngleY->value(), ui->doubleSpinBoxBoundingBoxAngleZ->value());
    });
    connect(ui->doubleSpinBoxBoundingBoxAngleY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxEulerAnglesValueChanged(ui->doubleSpinBoxBoundingBoxAngleX->value(), d, ui->doubleSpinBoxBoundingBoxAngleZ->value());
    });
    connect(ui->doubleSpinBoxBoundingBoxAngleZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double d) {
        mBoundingBoxManager->onBoundingBoxEulerAnglesValueChanged(ui->doubleSpinBoxBoundingBoxAngleX->value(), ui->doubleSpinBoxBoundingBoxAngleY->value(), d);
    });

    connect(ui->comboBoxBoundingBoxType, SIGNAL(currentIndexChanged(QString)), mBoundingBoxManager, SLOT(onComboBoxBoundingBoxTypeCurrentIndexChanged(QString)));

    connect(ui->pushButtonFinalizeBox, SIGNAL(clicked(bool)), mBoundingBoxManager, SLOT(onPushButtonFinalizeBoxClicked(bool)));
    connect(ui->pushButtonCancelBox, SIGNAL(clicked(bool)), mBoundingBoxManager, SLOT(onPushButtonCancelBoxClicked(bool)));
}

void MainWindow::setUpLOMConnections() {
    connect(this, SIGNAL(datasetChangingConfirmed(DatasetChangingConfirmedEventArgs&)), mLabelOverlayManager, SLOT(onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs&)));
    connect(this, SIGNAL(sceneChanged(SceneChangedEventArgs&)), mLabelOverlayManager, SLOT(onSceneChanged(SceneChangedEventArgs&)));

    connect(ui->checkBoxDisplayLabels, SIGNAL(stateChanged(int)), mLabelOverlayManager, SLOT(onCheckBoxDisplayLabelsStateChanged(int)));
    connect(ui->spinBoxMinLabelPx, SIGNAL(valueChanged(int)), mLabelOverlayManager, SLOT(onSpinBoxMinLabelPxValueChanged(int)));

    connect(ui->horizontalSliderLabelFontSize, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), [this](int value) {
        mLabelOverlayManager->onHorizontalSliderLabelFontSizeValueChanged(value * 5);
    });

    connect(ui->checkBoxDisplayLabelBorders, SIGNAL(stateChanged(int)), mLabelOverlayManager, SLOT(onCheckBoxDisplayLabelBordersStateChanged(int)));
}

QString MainWindow::buildWindowTitle() {
    QString title = Strings::WindowTitleBase + " - " + mDatasetManager->getDatasetDir().dirName();
    if(!mCurrentScene.getFileName().isEmpty())
        title += " - " + mCurrentScene.getFileName();
    return title;
}

void MainWindow::setBoundingBoxControlStates(bool enable) {
    QList<QDoubleSpinBox*> spinboxes = ui->groupBoxBoundingBoxes->findChildren<QDoubleSpinBox*>();
    QList<QComboBox*> comboboxes = ui->groupBoxBoundingBoxes->findChildren<QComboBox*>();

    for(QList<QDoubleSpinBox*>::iterator it = spinboxes.begin(); it != spinboxes.end(); ++it)
        (*it)->setEnabled(enable);
    for(QList<QComboBox*>::iterator it = comboboxes.begin(); it != comboboxes.end(); ++it)
        (*it)->setEnabled(enable);
    ui->pushButtonFinalizeBox->setEnabled(enable);
    ui->pushButtonCancelBox->setEnabled(enable);
    ui->pushButtonStartNewBox->setEnabled(!enable);
}

void MainWindow::restoreBoxDefaultsNoSignals() {
    QSignalBlocker b1(ui->doubleSpinBoxBoundingBoxCenterX);
    QSignalBlocker b2(ui->doubleSpinBoxBoundingBoxCenterY);
    QSignalBlocker b3(ui->doubleSpinBoxBoundingBoxCenterZ);
    QSignalBlocker b4(ui->doubleSpinBoxBoundingBoxExtentX);
    QSignalBlocker b5(ui->doubleSpinBoxBoundingBoxExtentY);
    QSignalBlocker b6(ui->doubleSpinBoxBoundingBoxExtentZ);
    QSignalBlocker b7(ui->doubleSpinBoxBoundingBoxAngleX);
    QSignalBlocker b8(ui->doubleSpinBoxBoundingBoxAngleY);
    QSignalBlocker b9(ui->doubleSpinBoxBoundingBoxAngleZ);
    QSignalBlocker b10(ui->comboBoxBoundingBoxType);

    ui->doubleSpinBoxBoundingBoxCenterX->setValue(Constants::InitialOBBCenter.x);
    ui->doubleSpinBoxBoundingBoxCenterY->setValue(Constants::InitialOBBCenter.y);
    ui->doubleSpinBoxBoundingBoxCenterZ->setValue(Constants::InitialOBBCenter.z);
    ui->doubleSpinBoxBoundingBoxExtentX->setValue(Constants::InitialOBBExtents.x);
    ui->doubleSpinBoxBoundingBoxExtentY->setValue(Constants::InitialOBBExtents.y);
    ui->doubleSpinBoxBoundingBoxExtentZ->setValue(Constants::InitialOBBExtents.z);
    ui->doubleSpinBoxBoundingBoxAngleX->setValue(Constants::InitialOBBEulerAngles.x);
    ui->doubleSpinBoxBoundingBoxAngleY->setValue(Constants::InitialOBBEulerAngles.y);
    ui->doubleSpinBoxBoundingBoxAngleZ->setValue(Constants::InitialOBBEulerAngles.z);
    ui->comboBoxBoundingBoxType->setCurrentIndex(Constants::InitialOBBObjectIndex);
}
