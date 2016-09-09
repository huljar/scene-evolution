#include <scene-evolution/NameProvider.h>

std::map<std::string, unsigned int> NameProvider::msIDs;

unsigned int NameProvider::nextID(const std::string& baseName) {
    if(!msIDs.count(baseName))
        msIDs[baseName] = 0;

    return msIDs[baseName]++;
}

unsigned int NameProvider::nextID(const QString& baseName) {
    return nextID(baseName.toStdString());
}
