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
    for(std::list<Query*>::iterator it = mResult.begin(); it != mResult.end(); ++it)
        delete *it;
}

void Driver::scanBegin() {
    yyset_debug(mTraceScanning);
    if(mFilePath.empty()) {
        // Read from string input
        mBufState = yy_scan_string(mScanString.c_str());
        yy_switch_to_buffer(mBufState);
    }
    else if(mFilePath == "-") {
        // Read from stdin
        //yyin = stdin;
        mBufState = yy_create_buffer(stdin, YY_BUF_SIZE);
        yy_switch_to_buffer(mBufState);
    }
    else {
        FILE* file = fopen(mFilePath.c_str(), "r");
        if(file) {
            mBufState = yy_create_buffer(file, YY_BUF_SIZE);
            yy_switch_to_buffer(mBufState);
        }
        else {
            error("Cannot open file: " + mFilePath);
            exit(EXIT_FAILURE); // TODO: change later
        }
    }
}

void Driver::scanEnd() {
    //fclose(yyin);
    yy_delete_buffer(mBufState);
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

int Driver::parseString(const QString& str) {
    // Delete old parsing tree
    for(std::list<Query*>::iterator it = mResult.begin(); it != mResult.end(); ++it)
        delete *it;
    mResult.clear();

    // Parse string
    mFilePath.clear();
    mScanString = str.toStdString();
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
