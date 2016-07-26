#ifndef DATASETMANAGER_H
#define DATASETMANAGER_H

#include <scene-evolution/Cache.h>
#include <scene-evolution/Scene.h>
#include <scene-evolution/CameraManager.h>

#include <opencv2/core/core.hpp>

#include <QDir>
#include <QException>
#include <QMap>
#include <QString>
#include <QVector>

#include <array>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>

class DatasetManager
{
public:
    typedef QMap<QString, unsigned short> LabelMap;

    class DatasetException : public std::runtime_error, public QException {
    public:
        explicit DatasetException(const std::string& what_arg);
        explicit DatasetException(const char* what_arg);
        explicit DatasetException(const QString& what_arg);

        // QException
        virtual void raise() const;
        virtual DatasetException* clone() const;
    };

    DatasetManager(const QDir& datasetDir);
    DatasetManager(const QString& datasetDirPath);

    bool checkIntegrity();

    QStringList getAllSceneNames();
    unsigned int getSceneCount();
    Scene loadScene(const QString& fileName);

    QDir getDatasetDir() const;
    void setDatasetDir(const QDir& datasetDir);
    void setDatasetDir(const QString& datasetDirPath);

    const QVector<QString>& getLabelNames() const;
    const LabelMap& getLabelMap() const;
    const CameraManager& getCameraParams() const;

protected:
    QVector<QString> readLabelFile(const QString& filePath);
    CameraManager readCameraFile(const QString& filePath);

    QDir mDatasetDir;
    QVector<QString> mLabelNames;
    LabelMap mLabelMap;
    CameraManager mCameraParams;

    Cache<QString, std::array<cv::Mat, 3>> mSceneCache;

private:
    void init();
};

#endif // DATASETMANAGER_H
