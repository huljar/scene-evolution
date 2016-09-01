#include <scene-evolution/EventArgs.h>

DatasetChangingEventArgs::DatasetChangingEventArgs(const QString& oldDatasetPath, const QString& newDatasetPath)
    : oldDatasetPath(oldDatasetPath)
    , newDatasetPath(newDatasetPath)
    , abort(false)
{
}

DatasetChangedEventArgs::DatasetChangedEventArgs(const QString& datasetPath, const QVector<QString>& labels)
    : datasetPath(datasetPath)
    , labels(labels)
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

SceneChangedEventArgs::SceneChangedEventArgs(const Scene& scene, unsigned int sceneIdx, RGBDScene* rgbdScene)
    : scene(scene)
    , sceneIdx(sceneIdx)
    , rgbdScene(rgbdScene)
{
}

WindowClosingEventArgs::WindowClosingEventArgs()
    : abort(false)
{
}
