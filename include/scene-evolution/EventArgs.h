#ifndef EVENTARGS_H
#define EVENTARGS_H

#include <QString>

struct DatasetChangingEventArgs {
    DatasetChangingEventArgs(const QString& oldDatasetPath, const QString& newDatasetPath);

    QString oldDatasetPath;
    QString newDatasetPath;

    bool abort;
};

struct DatasetChangedEventArgs {
    DatasetChangedEventArgs(const QString& datasetPath);

    QString datasetPath;
};

struct SceneChangingEventArgs {
    SceneChangingEventArgs(const QString& oldSceneName, unsigned int oldSceneIdx, const QString& newSceneName, unsigned int newSceneIdx);

    QString oldSceneName;
    unsigned int oldSceneIdx;

    QString newSceneName;
    unsigned int newSceneIdx;

    bool abort;
};

struct SceneChangedEventArgs {
    SceneChangedEventArgs(const QString& sceneName, unsigned int sceneIdx);

    QString sceneName;
    unsigned int sceneIdx;
};

struct WindowClosingEventArgs {
    WindowClosingEventArgs();

    bool abort;
};

#endif // EVENTARGS_H
