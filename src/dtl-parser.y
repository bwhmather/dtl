%skeleton "lalr1.cc"
%require "3.8.1"
%header

%define arg.value.type variant
%define api.namespace dtl
%define api.token.raw

%{
#include <memory>

#include "dtl-ast.hpp"

%}






%token Int
%token Float
%token Char
%token Byte
%token String
%token ByteString
%token RawString
%token RawByteString

/* Keywords. */
%token Begin
%token Update
%token Select
%token Distinct
%token Consecutive
%token As
%token From
%token Join
%token On
%token Using
%token Where
%token Group
%token By
%token With
%token Import
%token Export
%token To

/* Symbols */
%token Semicolon
%token Comma
%token Dot
%token OpenParen
%token CloseParen
%token OpenBrace
%token CloseBrace
%token OpenBracket
%token CloseBracket
%token At
%token Pound
%token Tilde
%token Question
%token Colon
%token Dollar

/* Operators */
%token Eq
%token Not
%token NotEqual
%token LessThan
%token LessThanEqual
%token GreaterThan
%token GreaterThanEqual
%token Minus
%token MinusEqual
%token And
%token Or
%token Plus
%token PlusEqual
%token Star
%token StarEqual
%token Slash
%token SlashEqual
%token Caret
%token Percent

    /* Identifiers */
%token Type
%token Name
%token QuotedName

    /* Blanks */
%token LineComment
%token BlockComment
%token Whitespace



%type <std::unique_ptr<Literal>> literal;
%type <std::unique_ptr<String>> string;

%type <std::unique_ptr<ColumnName>> column_name;
%type <std::unique_ptr<UnqualifiedColumnName>> unqualified_column_name;
%type <std::unique_ptr<QualifiedColumnName>> qualified_column_name;

%type <std::unique_ptr<Expression>> expression;
%type <std::unique_ptr<ColumnReferenceExpression>> column_reference_expression;
%type <std::unique_ptr<LiteralExpression>> literal_expression;
%type <std::unique_ptr<FunctionCallExpression>> function_call_expression;
%type <std::unique_ptr<AddExpression>> add_expression;
%type <std::unique_ptr<SubtractExpression>> subtract_expression;
%type <std::unique_ptr<MultiplyExpression>> multiply_expression;
%type <std::unique_ptr<DivideExpression>> divide_expression;

%type <std::unique_ptr<TableName>> table_name;
%type <std::unique_ptr<DistinctClause>> distinct_clause;

%type <std::unique_ptr<ColumnBinding>> column_binding;
%type <std::unique_ptr<WildcardColumnBinding>> wildcard_column_binding;
%type <std::unique_ptr<ImplicitColumnBinding>> implicit_column_binding;
%type <std::unique_ptr<AliasedColumnBinding>> aliased_column_binding;

%type <std::unique_ptr<FromClause>> from_clause;

%type <std::unique_ptr<JoinConstraint>> join_constraint;
%type <std::unique_ptr<JoinOnConstraint>> join_on_constraint;
%type <std::unique_ptr<JoinUsingConstraint>> join_using_constraint;

%type <std::unique_ptr<JoinClause>> join_clause;
%type <std::unique_ptr<WhereClause>> where_clause;
%type <std::unique_ptr<GroupByClause>> group_by_clause;

%type <std::unique_ptr<TableExpression>> table_expression;
%type <std::unique_ptr<SelectExpression>> select_expression;
%type <std::unique_ptr<ImportExpression>> import_expression;
%type <std::unique_ptr<TableReferenceExpression>> table_reference_expression;

%type <std::unique_ptr<Statement>> statement;
%type <std::unique_ptr<AssignmentStatement>> assignment_statement;
%type <std::unique_ptr<UpdateStatement>> update_statement;
%type <std::unique_ptr<DeleteStatement>> delete_statement;
%type <std::unique_ptr<InsertStatement>> insert_statement;
%type <std::unique_ptr<ExportStatement>> export_statement;

%type <std::unique_ptr<Script>> script;

%%

name
    : QuotedName
    | Name

literal
    : string
    ;

string 
    : String
    ;

column_name
    : unqualified_column_name
    | qualified_column_name
    ;

unqualified_column_name
    : name
    ;

qualified_column_name
    : name Dot name
    ;

expression
    : column_reference_expression
    | literal_expression
    | function_call_expression
    | add_expression
    | subtract_expression
    | multiply_expression
    | divide_expression
    ;

column_reference_expression
    : column_name
    ;

literal_expression
    : literal
    ;


function_call_arg_list
    : expression
    | function_call_arg_list Comma expression
    ;

function_call_expression
    : name OpenParen function_call_arg_list CloseParen
    ;

add_expression
    : expression Plus expression
    ;

subtract_expression
    : expression Minus expression
    ;

multiply_expression
    : expression Star expression
    ;

divide_expression
    : expression Slash expression
    ;

table_name
    : name
    ;

distinct_clause
    : Distinct
    | Distinct Consecutive
    | %empty
    ;

column_binding
    : wildcard_column_binding
    | implicit_column_binding
    | aliased_column_binding
    ;

wildcard_column_binding
    : Star
    ;

implicit_column_binding
    : expression
    ;

aliased_column_binding
    : expression As name
    ;

column_binding_list
    : column_binding
    | column_binding_list Comma column_binding
    ;

from_clause
    : From OpenParen table_expression CloseParen
    | From table_reference_expression
    | From OpenParen table_expression CloseParent As name
    | From table_reference_expression As name
    ;

aliased_table_binding
    : table_expression As name
    ;

join_constraint
    : join_on_constraint
    | join_using_constraint
    ;

join_on_constraint
    : On expression
    ;

column_name_list
    : unqualified_column_name
    | column_name_list Comma unqualified_column_name
    ;

join_using_constraint
    : Using OpenParen column_name_list CloseParen
    ;

join_clause
    : Join table_binding join_constraint
    ;

join_clause_list
    : %empty
    | join_clause_list join_clause
    ;

where_clause
    : Where expression
    | %empty
    ;

group_by_clause
    : Group By expression
    | Group Consecutive By expression
    | %empty
    ;

table_expression
    : select_expression
    | import_expression
    | table_reference_expression
    ;

select_expression
    : Select
        distinct_clause
        column_binding_list
        from_clause
        join_clause_list
        where_clause
        group_by_clause
    ; 

import_expression
    : Import string
    ;

table_reference_expression
    : name
    ;

statement
    : assignment_statement
//   | update_statement
//   | delete_statement
//    | insert_statement
    | export_statement
    ;

assignment_statement
    : name Eq expression Semicolon
    ;

export_statement
    : Export table_expression To string Semicolon
    ;

statement_list
    : %empty
    | statement_list statement
    ;

script
    : statement_list
    ;
    
%%
