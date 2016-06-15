#include <scene-evolution/EventArgs.h>

DatasetChangingEventArgs::DatasetChangingEventArgs(const QString& oldDatasetPath, const QString& newDatasetPath)
    : oldDatasetPath(oldDatasetPath)
    , newDatasetPath(newDatasetPath)
    , abort(false)
{
}

DatasetChangedEventArgs::DatasetChangedEventArgs(const QString& datasetPath)
    : datasetPath(datasetPath)
{
}

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

WindowClosingEventArgs::WindowClosingEventArgs()
    : abort(false)
{
}
