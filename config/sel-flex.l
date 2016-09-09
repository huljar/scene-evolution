%{ // C++
#include <SEL/sel-bison.h>
#include <QByteArray>
#include <QString>

// Define this here again instead of including SEL/Driver.h
// This is because including the driver also includes sel-flex.h which
// #undef's some macros required in sel-flex.cpp which causes compilation errors
#define YY_DECL SEL::Parser::symbol_type yylex(SEL::Driver& driver)

// The location of the current token.
static SEL::location loc;
%}

%option noyywrap nounput batch debug noinput

id      [a-zA-Z][a-zA-Z_0-9]*
float   -?[0-9]+\.[0-9]*
int     -?[0-9]+
blank   [ \t]

%{
// Code run each time a pattern is matched.
#define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
// Code run each time yylex is called.
loc.step();
%}

{blank}+            loc.step();
[\n]+               loc.lines(yyleng); loc.step();

(?i:select)         return SEL::Parser::make_SELECT(loc);
(?i:from\ scene)    return SEL::Parser::make_FROM(loc);
(?i:where)          return SEL::Parser::make_WHERE(loc);
(?i:distance\ to)   return SEL::Parser::make_DIST(loc);
(?i:supported\ by)  return SEL::Parser::make_SUPPORT(loc);
(?i:move\ to)       return SEL::Parser::make_MOVETO(loc);
(?i:move\ by)       return SEL::Parser::make_MOVEBY(loc);
(?i:rotate\ by)     return SEL::Parser::make_ROTBY(loc);
(?i:remove)         return SEL::Parser::make_REMOVE(loc);
(?i:copy\ at)       return SEL::Parser::make_COPYAT(loc);
(?i:or)             return SEL::Parser::make_OR(loc);
(?i:and)            return SEL::Parser::make_AND(loc);
(?i:not)            return SEL::Parser::make_NOT(loc);
"("                 return SEL::Parser::make_LPAREN(loc);
")"                 return SEL::Parser::make_RPAREN(loc);
","                 return SEL::Parser::make_COMMA(loc);
";"                 return SEL::Parser::make_SEMICOL(loc);
"="                 return SEL::Parser::make_EQ(loc);
"!="                return SEL::Parser::make_NE(loc);
"<"                 return SEL::Parser::make_LT(loc);
">"                 return SEL::Parser::make_GT(loc);
"<="                return SEL::Parser::make_LE(loc);
">="                return SEL::Parser::make_GE(loc);

(?i:rightmost)      return SEL::Parser::make_QUALIFIER("rightmost", loc);
(?i:leftmost)       return SEL::Parser::make_QUALIFIER("leftmost", loc);
(?i:topmost)        return SEL::Parser::make_QUALIFIER("topmost", loc);
(?i:bottommost)     return SEL::Parser::make_QUALIFIER("bottommost", loc);
(?i:all)            return SEL::Parser::make_QUALIFIER("all", loc);

(?i:true)           return SEL::Parser::make_BOOLEAN(true, loc);
(?i:false)          return SEL::Parser::make_BOOLEAN(false, loc);
{float}             return SEL::Parser::make_FLOAT(QByteArray(yytext).toFloat(), loc);
{int}               return SEL::Parser::make_INTEGER(QByteArray(yytext).toInt(), loc);

{id}                return SEL::Parser::make_IDENTIFIER(yytext, loc);
<<EOF>>             return SEL::Parser::make_END(loc);

%%
