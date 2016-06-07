#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <scene-evolution/CustomOgreWindow.h>

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

private:
    Ui::MainWindow* ui;

    CustomOgreWindow* mOgreWindow;
};

#endif // MAINWINDOW_H
