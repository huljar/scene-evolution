#ifndef SELDRIVER_H
#define SELDRIVER_H

#include <SEL/sel-bison.h>

#include <QString>
#include <QMap>

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

        // Error handling
        void error(const SEL::location& loc, const std::string& message);
        void error(const std::string& message);

        bool getTraceScanning() const;
        void setTraceScanning(bool traceScanning);

        bool getTraceParsing() const;
        void setTraceParsing(bool traceParsing);

        QString getFilePath() const;
        std::string* getFilePathPtr();

    protected:
        std::string mFilePath;

        bool mTraceScanning;
        bool mTraceParsing;
    };
}

#endif // SELDRIVER_H
