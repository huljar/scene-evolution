#ifndef SELDRIVER_H
#define SELDRIVER_H

#include <SEL/SELParser.h>

#include <QString>
#include <QMap>

#include <string>

// Tell Flex the lexer's prototype ...
#define YY_DECL SEL::SELParser::SymbolType yylex(SEL::SELDriver& driver)
// ... and declare it for the parser's sake
YY_DECL;

namespace SEL {
    class SELDriver
    {
    public:
        SELDriver();
        virtual ~SELDriver();

        // Scanner handling
        void scanBegin();
        void scanEnd();

        // Parser handling
        int parse(const QString& filePath);

        // Error handling
        void error(const SEL::location& loc, const std::string& message);
        void error(const std::string& message);

    protected:
        QString mFilePath;

        bool mTraceScanning;
        bool mTraceParsing;
    };
}

#endif // SELDRIVER_H
