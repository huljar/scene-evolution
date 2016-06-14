#ifndef EVENTARGS_H
#define EVENTARGS_H

#include <QString>

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

#endif // EVENTARGS_H
