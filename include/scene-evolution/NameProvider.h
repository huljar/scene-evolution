#ifndef NAMEPROVIDER_H
#define NAMEPROVIDER_H

#include <QString>

#include <map>
#include <string>

class NameProvider
{
public:
    NameProvider() = delete;

    static unsigned int nextID(const std::string& baseName);
    static unsigned int nextID(const QString& baseName);

protected:
    static std::map<std::string, unsigned int> msIDs;
};

#endif // NAMEPROVIDER_H
