#include <SEL/SELDriver.h>
#include <SEL/SELParser.h>

#include <iostream>

using namespace SEL;

SELDriver::SELDriver()
    : mTraceScanning(false)
    , mTraceParsing(false)
{
}

SELDriver::~SELDriver() {

}

void SELDriver::scanBegin() {

}

void SELDriver::scanEnd() {

}

int SELDriver::parse(const QString& filePath) {

}

void SELDriver::error(const SEL::location& loc, const std::string& message) {
    std::cerr << loc << ": " << message << std::endl;
}

void SELDriver::error(const std::string& message) {
    std::cerr << message << std::endl;
}
