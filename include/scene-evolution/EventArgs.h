#ifndef EVENTARGS_H
#define EVENTARGS_H

#include <scene-evolution/Scene.h>
#include <scene-evolution/RGBDScene.h>

#include <QString>
#include <QVector>

struct DatasetChangingEventArgs {
    DatasetChangingEventArgs(const QString& oldDatasetPath, const QString& newDatasetPath);

    QString oldDatasetPath;
    QString newDatasetPath;

    bool abort;
};

struct DatasetChangedEventArgs {
    DatasetChangedEventArgs(const QString& datasetPath, const QVector<QString>& labels);

    QString datasetPath;
    QVector<QString> labels;
};

struct SceneChangingEventArgs {
    SceneChangingEventArgs(const Scene& oldScene, unsigned int oldSceneIdx, const Scene& newScene, unsigned int newSceneIdx);

    Scene oldScene;
    unsigned int oldSceneIdx;

    Scene newScene;
    unsigned int newSceneIdx;

    bool abort;
};

struct SceneChangedEventArgs {
    SceneChangedEventArgs(const Scene& scene, unsigned int sceneIdx, RGBDScene* rgbdScene);

    Scene scene;
    unsigned int sceneIdx;
    RGBDScene* rgbdScene;
};

struct WindowClosingEventArgs {
    WindowClosingEventArgs();

    bool abort;
};

#endif // EVENTARGS_H
