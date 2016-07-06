%skeleton "lalr1.cc"                    // Request C++ usage
%require "3.0.4"                        // Require specific version for safety reasons

%defines                                // Generate a header file for the scanner to use

%define api.namespace {SEL}
%define parser_class_name {SELParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {                        // Code to put in the parser header file
#include <QString>
class SELDriver;                        // Forward declaration of driver class (to dissolve mutual dependency)
}

%param {SELDriver& driver}              // Add the parsing context as parameter to yylex/yyparse

%locations                              // Request location tracking
%initial-action {
    @$.begin.filename = &driver.file;
    @$.end.filename = &driver.file;     // Initialize the initial location to the file name from the driver
}

%define parse.trace                     // Enable parser tracing
%define parse.error verbose             // Enable verbose error messages (may contain incorrect information)

%code {                                 // Code to put in the parser implementation file
#include <SEL/SELDriver.h>
}

%define api.token.prefix {TOK_}         // Token prefix to avoid name clashes
%token                                  // Declare tokens without semantic values
    END 0   "end of file"
    SELECT  "select"
    FROM    "from"
    WHERE   "where"
    DIST    "distance to"
    SUPPORT "supported by"
    MOVE    "move to"
    REMOVE  "remove"
    LPAREN  "("
    RPAREN  ")"
    COMMA   ","
    SEMICOL ";"
    OR      "or"
    AND     "and"
    NOT     "not"
    EQ      "="
    NEQ     "!="
    LT      "<"
    GT      ">"
    LE      "<="
    GE      ">="
;

%token <QString> QUALIFIER "qualifier"  // Tokens with semantic values
%token <QString> IDENTIFIER "identifier"
%token <int> INTEGER "integer"
%token <float> FLOAT "float"

%type <Action> action
%type <std::list<Action>> action_list
%type <Object> object
%type <std::list<Object>> object_list // TODO: use list of pointers?
%type <std::list<Qualifier>> qualifier_list

%printer { yyoutput << $$; } <*>;

/*******************************/
%% // begin grammar definition //
/*******************************/

%start query_list;

query_list:
    query { $$ = std::list<Query>({$1}); }
  | query_list query { $1.push_back($2); $$ = $1; }; // TODO: use std::move()?

query:
    select_statement ";" action_list ";" { };

select_statement:
    "select" object_list "where" search_condition { };

object_list:
    object { $$ = std::list<Object>({$1}); }
  | object_list "," object { $1.push_back($2); $$ = $1; };

object:
    qualifier_list "identifier" { };

qualifier_list:
    { $$ = std::list<Qualifier>(); }
  | qualifier_list "qualifier" { $1.push_back($2); $$ = $1; };

search_condition:
    boolean_term { }
  | search_condition "or" boolean_term { };

boolean_term:
    boolean_factor { }
  | boolean_term "and" boolean_factor { };

boolean_factor:
    boolean_test { }
  | "not" boolean_test { };

boolean_test:
    predicate { }
  | "(" search_condition ")" { };

predicate:
    comp_predicate { }
  | binary_predicate { };

comp_predicate:
    comp_element "=" comp_element { }
  | comp_element "!=" comp_element { }
  | comp_element "<" comp_element { }
  | comp_element ">" comp_element { }
  | comp_element "<=" comp_element { }
  | comp_element ">=" comp_element { };

comp_element:
    numeric_value { };
  | distance_term { };

distance_term:
    "distance to" object { };

binary_predicate:
    support_term { };

support_term:
    "supported by" object { };

numeric_value:
    "integer" { }
  | "float" { };

action_list:
    action_statement { $$ = std::list<Action>({$1}); }
  | action_list action_statement { $1.push_back($2); $$ = $1; };

action_statement:
    move_statement { }
  | remove_statement { };

move_statement:
    "move to" object "where" search_condition { };

remove_statement:
    "remove" { };


/*****************************/
%% // end grammar definition //
/*****************************/

void SEL::SELParser::error(const location_type& loc, const std::string& message) {
    driver.error(loc, message);
}
