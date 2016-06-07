#ifndef CUSTOMOGREWINDOW_H
#define CUSTOMOGREWINDOW_H

#include <scene-evolution/QtOgreWindow.h>

class CustomOgreWindow : public QTOgreWindow
{
public:
    explicit CustomOgreWindow(QWindow *parent = NULL);
    virtual ~CustomOgreWindow();

    void createScene() override;

protected:
    void createCamera() override;
};

#endif // CUSTOMOGREWINDOW_H
