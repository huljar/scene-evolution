%skeleton "lalr1.cc"                    // Request C++ usage
%require "3.0.4"                        // Require specific version for safety reasons

%define api.namespace {SEL}
%define parser_class_name {Parser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

// Code to put in the parser header file
%code requires {
namespace SEL {
    class Driver;                       // Forward declaration of driver class (to dissolve mutual dependency)
}

#include <SEL/Query.h>
#include <SEL/SelectStatement.h>
#include <SEL/Object.h>
#include <SEL/Qualifier.h>
#include <SEL/SearchCondition.h>
#include <SEL/BooleanTerm.h>
#include <SEL/BooleanFactor.h>
#include <SEL/BooleanTest.h>
#include <SEL/Predicate.h>
#include <SEL/CompPredicate.h>
#include <SEL/CompElement.h>
#include <SEL/DistanceTerm.h>
#include <SEL/BinaryPredicate.h>
#include <SEL/SupportTerm.h>
#include <SEL/NumericValue.h>
#include <SEL/BooleanValue.h>
#include <SEL/Action.h>
#include <SEL/MoveAction.h>
#include <SEL/RemoveAction.h>
}

%param {SEL::Driver& driver}            // Add the parsing context as parameter to yylex/yyparse

%locations                              // Request location tracking

// Initialize the initial location to the file name from the driver
%initial-action {
    @$.begin.filename = driver.getFilePathPtr();
    @$.end.filename = driver.getFilePathPtr();
}

%define parse.trace                     // Enable parser tracing
%define parse.error verbose             // Enable verbose error messages (may contain incorrect information)

// Code to put in the parser implementation file
%code {
#include <SEL/Driver.h>                 // Here we do not need to worry about mutual inclusion
#include <QString>
#include <list>
}

%define api.token.prefix {TOK_}         // Token prefix to avoid name clashes

// Declare tokens without semantic values
%token
    END 0   "end of file"
    SELECT  "select"
    FROM    "from"
    WHERE   "where"
    DIST    "distance to"
    SUPPORT "supported by"
    MOVE    "move to"
    REMOVE  "remove"
    OR      "or"
    AND     "and"
    NOT     "not"
    LPAREN  "("
    RPAREN  ")"
    COMMA   ","
    SEMICOL ";"
    EQ      "="
    NE      "!="
    LT      "<"
    GT      ">"
    LE      "<="
    GE      ">="
;

// Declare tokens with semantic values
%token <QString> QUALIFIER "qualifier"
%token <QString> IDENTIFIER "identifier"
%token <int> INTEGER "integer"
%token <float> FLOAT "float"
%token <bool> BOOLEAN "boolean"

// Declare types of non-terminals
%type <std::list<SEL::Query*>> query_list
%type <SEL::Query*> query
%type <SEL::SelectStatement*> select_statement
%type <std::list<SEL::Object*>> object_list
%type <SEL::Object*> object
%type <std::list<SEL::Qualifier*>> qualifier_list
%type <SEL::Qualifier*> qualifier
%type <SEL::SearchCondition*> search_condition
%type <SEL::BooleanTerm*> boolean_term
%type <SEL::BooleanFactor*> boolean_factor
%type <SEL::BooleanTest*> boolean_test
%type <SEL::Predicate*> predicate
%type <SEL::CompPredicate*> comp_predicate
%type <SEL::CompElement*> comp_element
%type <SEL::DistanceTerm*> distance_term
%type <SEL::BinaryPredicate*> binary_predicate
%type <SEL::SupportTerm*> support_term
%type <SEL::NumericValue*> numeric_value
%type <SEL::BooleanValue*> boolean_value
%type <std::list<SEL::Action*>> action_list
%type <SEL::Action*> action
%type <SEL::MoveAction*> move_action
%type <SEL::RemoveAction*> remove_action

%destructor { delete $$; } <*>
%destructor { for(auto it = $$.begin(); it != $$.end(); ++it) delete *it; } query_list object_list qualifier_list action_list
%destructor { } "qualifier" "identifier" "integer" "float" "boolean"

%printer { yyoutput << *$$; } action object;

/*******************************/
%% // begin grammar definition //
/*******************************/

query_list:
    query { $$ = std::list<SEL::Query*>({$1}); }
  | query_list query { $1.push_back($2); $$ = std::move($1); };

query:
    select_statement ";" action_list ";" { $$ = new SEL::Query($1, $3); };

select_statement:
    "select" object_list "where" search_condition { $$ = new SEL::SelectStatement($2, $4); };

object_list:
    object { $$ = std::list<SEL::Object*>({$1}); }
  | object_list "," object { $1.push_back($3); $$ = std::move($1); };

object:
    qualifier_list "identifier" { $$ = new SEL::Object($1, $2); };

qualifier_list:
    { $$ = std::list<SEL::Qualifier*>(); }
  | qualifier_list qualifier { $1.push_back($2); $$ = $1; };

qualifier:
    "qualifier" { $$ = new SEL::Qualifier($1); };

search_condition:
    boolean_term { $$ = new SEL::SearchCondition(nullptr, $1); }
  | search_condition "or" boolean_term { $$ = new SEL::SearchCondition($1, $3); };

boolean_term:
    boolean_factor { $$ = new SEL::BooleanTerm(nullptr, $1); }
  | boolean_term "and" boolean_factor { $$ = new SEL::BooleanTerm($1, $3); };

boolean_factor:
    boolean_test { $$ = new SEL::BooleanFactor($1, false); }
  | "not" boolean_test { $$ = new SEL::BooleanFactor($2, true); };

boolean_test:
    predicate { $$ = new SEL::BooleanTest($1); }
  | "(" search_condition ")" { $$ = new SEL::BooleanTest($2); };

predicate:
    comp_predicate { $$ = $1; }
  | binary_predicate { $$ = $1; };

comp_predicate:
    comp_element "=" comp_element { $$ = new SEL::CompPredicate($1, SEL::CompPredicate::Operator::EQ, $3); }
  | comp_element "!=" comp_element { $$ = new SEL::CompPredicate($1, SEL::CompPredicate::Operator::NE, $3); }
  | comp_element "<" comp_element { $$ = new SEL::CompPredicate($1, SEL::CompPredicate::Operator::LT, $3); }
  | comp_element ">" comp_element { $$ = new SEL::CompPredicate($1, SEL::CompPredicate::Operator::GT, $3); }
  | comp_element "<=" comp_element { $$ = new SEL::CompPredicate($1, SEL::CompPredicate::Operator::LE, $3); }
  | comp_element ">=" comp_element { $$ = new SEL::CompPredicate($1, SEL::CompPredicate::Operator::GE, $3); };

comp_element:
    numeric_value { $$ = $1; }
  | distance_term { $$ = $1; };

distance_term:
    "distance to" object { $$ = new SEL::DistanceTerm($2); };

binary_predicate:
    boolean_value { $$ = $1; }
  | support_term { $$ = $1; };

support_term:
    "supported by" object { $$ = new SEL::SupportTerm($2); };

numeric_value:
    "integer" { $$ = new SEL::NumericValue(QVariant($1)); }
  | "float" { $$ = new SEL::NumericValue(QVariant($1)); };

boolean_value:
    "boolean" { $$ = new SEL::BooleanValue($1); };

action_list:
    action { $$ = std::list<Action*>({$1}); }
  | action_list action { $1.push_back($2); $$ = $1; };

action:
    move_action { $$ = $1; }
  | remove_action { $$ = $1; };

move_action:
    "move to" object "where" search_condition { $$ = new SEL::MoveAction($2, $4); };

remove_action:
    "remove" { $$ = new SEL::RemoveAction(); };

/*****************************/
%% // end grammar definition //
/*****************************/

void SEL::Parser::error(const location& loc, const std::string& message) {
    driver.error(loc, message);
}
