#include <scene-evolution/MainWindow.h>
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mOgreWindow(new CustomOgreWindow)
{
    ui->setupUi(this);

    // Load OGRE context
    QWidget* ogreContainer = QWidget::createWindowContainer(mOgreWindow);
    ui->widgetOgreContainer->layout()->addWidget(ogreContainer);
}

MainWindow::~MainWindow() {
    delete ui;
}
