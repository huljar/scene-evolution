#include <scene-evolution/EventArgs.h>

SceneChangingEventArgs::SceneChangingEventArgs(const QString& oldSceneName, unsigned int oldSceneIdx, const QString& newSceneName, unsigned int newSceneIdx)
    : oldSceneName(oldSceneName)
    , oldSceneIdx(oldSceneIdx)
    , newSceneName(newSceneName)
    , newSceneIdx(newSceneIdx)
    , abort(false)
{

}

SceneChangedEventArgs::SceneChangedEventArgs(const QString& sceneName, unsigned int sceneIdx)
    : sceneName(sceneName)
    , sceneIdx(sceneIdx)
{

}
