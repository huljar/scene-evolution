#ifndef SELDRIVER_H
#define SELDRIVER_H

#include <SEL/sel-flex.h>
#include <SEL/sel-bison.h>
#include <SEL/Query.h>

#include <QString>
#include <QMap>

#include <list>
#include <string>

// Tell Flex the lexer's prototype ...
#define YY_DECL SEL::Parser::symbol_type yylex(SEL::Driver& driver)
// ... and declare it for the parser's sake
YY_DECL;

namespace SEL {
    class Driver
    {
    public:
        Driver();
        virtual ~Driver();

        // Scanner handling
        void scanBegin();
        void scanEnd();

        // Parser handling
        int parse(const QString& filePath);
        int parseString(const QString& str);

        // Error handling
        void error(const SEL::location& loc, const std::string& message);
        void error(const std::string& message);

        bool getTraceScanning() const;
        void setTraceScanning(bool traceScanning);

        bool getTraceParsing() const;
        void setTraceParsing(bool traceParsing);

        QString getFilePath() const;
        std::string* getFilePathPtr();

        std::list<Query*> getResult() const;
        void setResult(const std::list<Query*>& result);
        void setResult(std::list<Query*>&& result);

    protected:
        std::string mFilePath;
        std::string mScanString;

        YY_BUFFER_STATE mBufState;

        bool mTraceScanning;
        bool mTraceParsing;

        std::list<Query*> mResult;
    };
}

#endif // SELDRIVER_H
