#include <SEL/Driver.h>
#include <SEL/sel-bison.h>
#include <SEL/sel-flex.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace SEL;

Driver::Driver()
    : mTraceScanning(false)
    , mTraceParsing(false)
{
}

Driver::~Driver() {

}

void Driver::scanBegin() {
    yyset_debug(mTraceScanning);
    if(mFilePath.empty() || mFilePath == "-") {
        yyin = stdin;
    }
    else if(!(yyin = fopen(mFilePath.c_str(), "r"))) {
        error("Cannot open file: " + mFilePath);
        exit(EXIT_FAILURE); // TODO: change later
    }
}

void Driver::scanEnd() {
    fclose(yyin);
}

int Driver::parse(const QString& filePath) {
    // Delete old parsing tree
    for(std::list<Query*>::iterator it = mResult.begin(); it != mResult.end(); ++it)
        delete *it;
    mResult.clear();

    // Parse new file
    mFilePath = filePath.toStdString();
    scanBegin();
    SEL::Parser parser(*this);
    parser.set_debug_level(mTraceParsing);
    int res = parser.parse();
    scanEnd();
    return res;
}

void Driver::error(const SEL::location& loc, const std::string& message) {
    std::cerr << loc << ": " << message << std::endl;
}

void Driver::error(const std::string& message) {
    std::cerr << message << std::endl;
}

bool Driver::getTraceScanning() const {
    return mTraceScanning;
}

void Driver::setTraceScanning(bool traceScanning) {
    mTraceScanning = traceScanning;
}

bool Driver::getTraceParsing() const {
    return mTraceParsing;
}

void Driver::setTraceParsing(bool traceParsing) {
    mTraceParsing = traceParsing;
}

QString Driver::getFilePath() const {
    return QString::fromStdString(mFilePath);
}

std::string* Driver::getFilePathPtr() {
    return &mFilePath;
}

std::list<Query*> Driver::getResult() const {
    return mResult;
}

void Driver::setResult(const std::list<Query*>& result) {
    mResult = result;
}

void Driver::setResult(std::list<Query*>&& result) {
    mResult = result;
}
