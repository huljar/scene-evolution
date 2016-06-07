#include <scene-evolution/DatasetManager.h>

#include <opencv2/highgui/highgui.hpp>

#include <QFile>
#include <QTextStream>

#include <iostream>

DatasetManager::DatasetManager(const QDir& datasetDir)
    : mDatasetDir(datasetDir)
    , mSceneCache(100)
{
    init();
}

DatasetManager::DatasetManager(const QString& datasetDirPath)
    : mDatasetDir(QDir(datasetDirPath))
    , mSceneCache(100)
{
    init();
}

bool DatasetManager::checkIntegrity() {
    if(!mDatasetDir.exists())
        return false;

    QDir depthDir(mDatasetDir.filePath("depth"));
    QDir rgbDir(mDatasetDir.filePath("rgb"));
    QDir labelDir(mDatasetDir.filePath("label"));

    QFile paramFile(mDatasetDir.filePath("Camera-Parameter.txt"));
    QFile labelFile(mDatasetDir.filePath("labelNames.txt"));

    return depthDir.exists() && rgbDir.exists() && labelDir.exists() && paramFile.exists() && labelFile.exists()
            && depthDir.count() == rgbDir.count() && depthDir.count() == labelDir.count() && depthDir.count() > 0;
}

QStringList DatasetManager::getAllSceneNames() {
    if(!checkIntegrity())
        throw DatasetException("Integrity check failed");

    return QDir(mDatasetDir.filePath("depth")).entryList(QDir::Files, QDir::Name);
}

unsigned int DatasetManager::getSceneCount() {
    if(!checkIntegrity())
        throw DatasetException("Integrity check failed");

    return static_cast<unsigned int>(QDir(mDatasetDir.filePath("depth")).entryList(QDir::Files).size());
}

Scene DatasetManager::loadScene(const QString& fileName) {
    auto pos = mSceneCache.load(fileName);
    if(mSceneCache.isValid(pos))
        return Scene(pos->second[0], pos->second[1], pos->second[2], fileName);

    cv::Mat depthTmp = cv::imread(QDir(mDatasetDir.filePath("depth")).filePath(fileName).toStdString(), CV_LOAD_IMAGE_UNCHANGED);
    cv::Mat rgbTmp = cv::imread(QDir(mDatasetDir.filePath("rgb")).filePath(fileName).toStdString(), CV_LOAD_IMAGE_COLOR);
    cv::Mat labelTmp = cv::imread(QDir(mDatasetDir.filePath("label")).filePath(fileName).toStdString(), CV_LOAD_IMAGE_UNCHANGED);

    if(!(depthTmp.data && rgbTmp.data && labelTmp.data))
        throw DatasetException("Unable to read scene from disk");

    mSceneCache.insert(fileName, std::array<cv::Mat, 3>({depthTmp, rgbTmp, labelTmp}));

    return Scene(depthTmp, rgbTmp, labelTmp, fileName);
}

QDir DatasetManager::getDatasetDir() const {
    return mDatasetDir;
}

void DatasetManager::setDatasetDir(const QDir& datasetDir) {
    mDatasetDir.cd(datasetDir.path());
}

void DatasetManager::setDatasetDir(const QString& datasetDirPath) {
    mDatasetDir.cd(datasetDirPath);
}

QVector<QString> DatasetManager::readLabelFile(const QString& filePath) {
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "Error: Unable to open label file stream!" << std::endl;
        return QVector<QString>();
    }

    QVector<QString> ret;
    ret.push_back("unlabeled");

    QTextStream in(&file);
    QString line;

    while(in.readLineInto(&line)) {
        if(line.length() > 2 && line[0] == '\'' && line[line.length() - 1] == '\'') {
            ret.push_back(line.mid(1, line.length() - 2));
        }
    }

    return ret;
}

const QVector<QString>& DatasetManager::getLabelNames() const {
    return mLabelNames;
}

void DatasetManager::init() {
    mLabelNames = readLabelFile(mDatasetDir.filePath("labelNames.txt"));
}

DatasetManager::DatasetException::DatasetException(const std::string& what_arg)
    : std::runtime_error(what_arg)
{
}

DatasetManager::DatasetException::DatasetException(const char* what_arg)
    : std::runtime_error(what_arg)
{
}

DatasetManager::DatasetException::DatasetException(const QString& what_arg)
    : std::runtime_error(what_arg.toStdString())
{
}

void DatasetManager::DatasetException::raise() const {
    throw *this;
}

DatasetManager::DatasetException* DatasetManager::DatasetException::clone() const {
    return new DatasetException(*this);
}
