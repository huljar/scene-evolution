#ifndef CUSTOMOGREWINDOW_H
#define CUSTOMOGREWINDOW_H

#include <scene-evolution/QtOgreWindow.h>

#include <OGRE/Overlay/OgreOverlaySystem.h>

class CustomOgreWindow : public QTOgreWindow
{
    Q_OBJECT

public:
    explicit CustomOgreWindow(QWindow *parent = NULL);
    virtual ~CustomOgreWindow();

    void createScene() override;

protected:
    void preInitSetup() override;
    void postInitSetup() override;
    void createCamera() override;
    void keyPressEvent(QKeyEvent* e) override;

    Ogre::OverlaySystem* mOverlaySystem;
};

#endif // CUSTOMOGREWINDOW_H
