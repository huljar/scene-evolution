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

    file.close();

    return ret;
}

CameraManager DatasetManager::readCameraFile(const QString& filePath) {
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "Error: Unable to open camera file stream!" << std::endl;
        return CameraManager();
    }

    QTextStream in(&file);
    QString line;

    cv::Vec2f depthPrincipalPoint;
    cv::Vec2f depthFocalLength;
    cv::Vec2f rgbPrincipalPoint;
    cv::Vec2f rgbFocalLength;
    cv::Matx33f rotation;
    cv::Vec3f translation;
    CameraManager::MapMode mapMode = CameraManager::MapMode::MAPPED_DEPTH_TO_RGB;

    // TODO: implement actual parsing
    in.readLineInto(&line);

    if(line.startsWith("%% Color")) { // NYU V1
        depthPrincipalPoint = cv::Vec2f(3.2442516903961865e+02, 2.3584766381177013e+02);
        depthFocalLength = cv::Vec2f(5.7616540758591043e+02, 5.7375619782082447e+02);
        rgbPrincipalPoint = cv::Vec2f(3.2850951551345941e+02, 2.5282555217253503e+02);
        rgbFocalLength = cv::Vec2f(5.1930334103339817e+02, 5.1816401430246583e+02);
        rotation = cv::Matx33f(9.9998579449446667e-01, 3.4203777687649762e-03, -4.0880099301915437e-03,
                               -3.4291385577729263e-03, 9.9999183503355726e-01, -2.1379604698021303e-03,
                               4.0806639192662465e-03, 2.1519484514690057e-03,  9.9998935859330040e-01);
        translation = cv::Vec3f(2.2142187053089738e-02, -1.4391632009665779e-04, -7.9356552371601212e-03);
    }
    else if(line.startsWith("% Calibrated using the")) { // NYU V2
        depthPrincipalPoint = cv::Vec2f(3.1304475870804731e+02, 2.3844389626620386e+02);
        depthFocalLength = cv::Vec2f(5.8262448167737955e+02, 5.8269103270988637e+02);
        rgbPrincipalPoint = cv::Vec2f(3.2558244941119034e+02, 2.5373616633400465e+02);
        rgbFocalLength = cv::Vec2f(5.1885790117450188e+02, 5.1946961112127485e+02);
        rotation = cv::Matx33f(9.9997798940829263e-01, 5.0518419386157446e-03, 4.3011152014118693e-03,
                               -5.0359919480810989e-03, 9.9998051861143999e-01, -3.6879781309514218e-03,
                               -4.3196624923060242e-03, 3.6662365748484798e-03, 9.9998394948385538e-01);
        translation = cv::Vec3f(2.5031875059141302e-02, -2.9342312935846411e-04, 6.6238747008330102e-04);
    }
    else {
        std::cerr << "Error: Unrecognized camera parameter file!" << std::endl;
    }

    file.close();

    return CameraManager(depthPrincipalPoint, depthFocalLength, rgbPrincipalPoint, rgbFocalLength, rotation, translation, mapMode);
}

const DatasetManager::LabelMap& DatasetManager::getLabelMap() const {
    return mLabelMap;
}

const QVector<QString>& DatasetManager::getLabelNames() const {
    return mLabelNames;
}

const CameraManager& DatasetManager::getCameraParams() const {
    return mCameraParams;
}

void DatasetManager::init() {
    mLabelNames = readLabelFile(mDatasetDir.filePath("labelNames.txt"));
    mCameraParams = readCameraFile(mDatasetDir.filePath("Camera-Parameter.txt"));

    for(int i = 0; i < mLabelNames.size(); ++i) {
        mLabelMap.insert(mLabelNames.at(i), static_cast<unsigned short>(i));
    }
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
