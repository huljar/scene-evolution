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

SceneChangingEventArgs::SceneChangingEventArgs(const Scene& oldScene, unsigned int oldSceneIdx, const Scene& newScene, unsigned int newSceneIdx)
    : oldScene(oldScene)
    , oldSceneIdx(oldSceneIdx)
    , newScene(newScene)
    , newSceneIdx(newSceneIdx)
    , abort(false)
{
}

SceneChangedEventArgs::SceneChangedEventArgs(const Scene& scene, unsigned int sceneIdx)
    : scene(scene)
    , sceneIdx(sceneIdx)
{
}

WindowClosingEventArgs::WindowClosingEventArgs()
    : abort(false)
{
}
