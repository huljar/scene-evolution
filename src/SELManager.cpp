#include <SEL/SELManager.h>
#include <SEL/sel-flex.h>
#include <SEL/sel-bison.h>

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include <stdio.h>

SELManager::SELManager(const QString& defaultSavePath)
    : mFilePath(defaultSavePath)
{

}

bool SELManager::loadFromFile() {
    // Get file path
    QString path = QFileDialog::getOpenFileName(nullptr, tr("Load File"), (mFilePath.isEmpty() ? QDir::homePath() : mFilePath),
                                                tr("Scene Evolution Language files (*.sel)"));

    if(path.isEmpty())
        return false;

    if(!loadFromFile(path)) {
        QMessageBox::warning(nullptr, tr("Scene Evolution"), tr("Unable to load specified file."), QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    return true;
}

bool SELManager::loadFromFile(const QString& path) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    // Convert QFile to C-style FILE*
    int fileHandle = file.handle();
    FILE* cfile = fdopen(fileHandle, "r");
    if(!cfile)
        return false;

    // Register file with Flex
    yyin = cfile;

    // Parse through the input until there is no more:
    yylex();

    return true;
}
