#ifndef SELMANAGER_H
#define SELMANAGER_H

#include <QObject>
#include <QString>

class SELManager : public QObject
{
    Q_OBJECT

public:
    SELManager(const QString& defaultSavePath = QString());

    bool loadFromFile();
    bool loadFromFile(const QString& path);

private:
    QString mFilePath;
};

#endif // SELMANAGER_H
