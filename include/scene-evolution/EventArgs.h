#ifndef EVENTARGS_H
#define EVENTARGS_H

#include <QString>

class SceneChangingEventArgs
{
public:
    SceneChangingEventArgs(const QString& oldSceneName, unsigned int oldSceneIdx, const QString& newSceneName, unsigned int newSceneIdx);

    QString oldSceneName;
    unsigned int oldSceneIdx;

    QString newSceneName;
    unsigned int newSceneIdx;

    bool abort;
};

class SceneChangedEventArgs
{
public:
    SceneChangedEventArgs(const QString& sceneName, unsigned int sceneIdx);

    QString sceneName;
    unsigned int sceneIdx;
};

#endif // EVENTARGS_H
