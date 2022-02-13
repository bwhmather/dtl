%skeleton "lalr1.cc"
%require "3.8.1"
%header

%define api.value.type variant
%define api.namespace dtl
%define api.token.raw

%code requires {
#include <memory>
#include <locale>
#include <vector>
#include <iostream>

#include "dtl-ast.hpp"

}

%token INT
%token FLOAT
%token STRING
%token BYTE_STRING

/* Keywords. */
%token BEGIN
%token UPDATE
%token SELECT
%token DISTINCT
%token CONSECUTIVE
%token AS
%token FROM
%token JOIN
%token ON
%token USING
%token WHERE
%token GROUP
%token BY
%token WITH
%token IMPORT
%token EXPORT
%token TO

/* Symbols */
%token SEMICOLON
%token COMMA
%token DOT
%token OPEN_PAREN
%token CLOSE_PAREN
%token OPEN_BRACE
%token CLOSE_BRACE
%token OPEN_BRACKET
%token CLOSE_BRACKET
%token AT
%token POUND
%token TILDE
%token QUESTION
%token COLON
%token DOLLAR

/* Operators */
%token EQ
%token NOT
%token NOT_EQUAL
%token LESS_THAN
%token LESS_THAN_EQUAL
%token GREATER_THAN
%token GREATER_THAN_EQUAL
%token MINUS
%token PLUS
%token STAR
%token SLASH
%token CARET
%token PERCENT

/* Identifiers */
%token TYPE
%token <std::string> NAME
%token <std::string> QUOTED_NAME

/* Blanks */
%token LINE_COMMENT
%token BLOCK_COMMENT
%token WHITESPACE

%left PLUS MINUS
%left STAR SLASH

%type <std::string> name;

%type <std::unique_ptr<dtl::ast::Literal>> literal;
%type <std::unique_ptr<dtl::ast::String>> string;

%type <std::unique_ptr<dtl::ast::ColumnName>> column_name;
%type <std::vector<std::unique_ptr<dtl::ast::UnqualifiedColumnName>>> unqualified_column_name_list;
%type <std::unique_ptr<dtl::ast::UnqualifiedColumnName>> unqualified_column_name;
%type <std::unique_ptr<dtl::ast::QualifiedColumnName>> qualified_column_name;

%type <std::unique_ptr<dtl::ast::Expression>> expression;
%type <std::unique_ptr<dtl::ast::ColumnReferenceExpression>> column_reference_expression;
%type <std::unique_ptr<dtl::ast::LiteralExpression>> literal_expression;
%type <std::vector<std::unique_ptr<dtl::ast::Expression>>> expression_list;
%type <std::unique_ptr<dtl::ast::FunctionCallExpression>> function_call_expression;
%type <std::unique_ptr<dtl::ast::AddExpression>> add_expression;
%type <std::unique_ptr<dtl::ast::SubtractExpression>> subtract_expression;
%type <std::unique_ptr<dtl::ast::MultiplyExpression>> multiply_expression;
%type <std::unique_ptr<dtl::ast::DivideExpression>> divide_expression;

%type <std::unique_ptr<dtl::ast::TableName>> table_name;
%type <std::unique_ptr<dtl::ast::DistinctClause>> distinct_clause;

%type <std::unique_ptr<dtl::ast::ColumnBinding>> column_binding;
%type <std::vector<std::unique_ptr<dtl::ast::ColumnBinding>>> column_binding_list;
%type <std::unique_ptr<dtl::ast::WildcardColumnBinding>> wildcard_column_binding;
%type <std::unique_ptr<dtl::ast::ImplicitColumnBinding>> implicit_column_binding;
%type <std::unique_ptr<dtl::ast::AliasedColumnBinding>> aliased_column_binding;

%type <std::unique_ptr<dtl::ast::TableBinding>> table_binding;
%type <std::unique_ptr<dtl::ast::ImplicitTableBinding>> implicit_table_binding;
%type <std::unique_ptr<dtl::ast::AliasedTableBinding>> aliased_table_binding;

%type <std::unique_ptr<dtl::ast::FromClause>> from_clause;

%type <std::unique_ptr<dtl::ast::JoinConstraint>> join_constraint;
%type <std::unique_ptr<dtl::ast::JoinOnConstraint>> join_on_constraint;
%type <std::unique_ptr<dtl::ast::JoinUsingConstraint>> join_using_constraint;

%type <std::unique_ptr<dtl::ast::JoinClause>> join_clause;
%type <std::vector<std::unique_ptr<dtl::ast::JoinClause>>> join_clause_list;

%type <std::unique_ptr<dtl::ast::WhereClause>> where_clause;
%type <std::unique_ptr<dtl::ast::GroupByClause>> group_by_clause;

%type <std::unique_ptr<dtl::ast::TableExpression>> table_expression;
%type <std::unique_ptr<dtl::ast::SelectExpression>> select_expression;
%type <std::unique_ptr<dtl::ast::ImportExpression>> import_expression;
%type <std::unique_ptr<dtl::ast::TableReferenceExpression>> table_reference_expression;

%type <std::unique_ptr<dtl::ast::Statement>> statement;
%type <std::vector<std::unique_ptr<dtl::ast::Statement>>> statement_list;
%type <std::unique_ptr<dtl::ast::AssignmentStatement>> assignment_statement;
//%type <std::unique_ptr<dtl::ast::UpdateStatement>> update_statement;
//%type <std::unique_ptr<dtl::ast::DeleteStatement>> delete_statement;
//%type <std::unique_ptr<dtl::ast::InsertStatement>> insert_statement;
%type <std::unique_ptr<dtl::ast::ExportStatement>> export_statement;

%type <std::unique_ptr<dtl::ast::Script>> script;

%%
%start script;

name
    : QUOTED_NAME
    | NAME
    ;

literal
    : string { $$ = std::move($1); }
    ;

string
    : STRING {

    }
    ;

column_name
    : unqualified_column_name {
        $$ = std::move($1);
    }
    | qualified_column_name {
        $$ = std::move($1);
    }
    ;

unqualified_column_name
    : name[column_name] {
        $$ = std::make_unique<dtl::ast::UnqualifiedColumnName>();
        $$->column_name = std::move($column_name);
    }
    ;

qualified_column_name
    : name[table_name] DOT name[column_name] {
        $$ = std::make_unique<dtl::ast::QualifiedColumnName>();
        $$->table_name = std::move($table_name);
        $$->column_name = std::move($column_name);
    }
    ;

expression
    : column_reference_expression {
        $$ = std::move($1);
    }
    | literal_expression {
        $$ = std::move($1);
    }
    | function_call_expression {
        $$ = std::move($1);
    }
    | add_expression {
        $$ = std::move($1);
    }
    | subtract_expression {
        $$ = std::move($1);
    }
    | multiply_expression {
        $$ = std::move($1);
    }
    | divide_expression {
        $$ = std::move($1);
    }
    ;

column_reference_expression
    : column_name[name] {
        $$ = std::make_unique<dtl::ast::ColumnReferenceExpression>();
        $$->name = std::move($name);
    }
    ;

literal_expression
    : literal[value] {
        $$ = std::make_unique<dtl::ast::LiteralExpression>();
        $$->value = std::move($value);
    }
    ;

expression_list
    : expression[head] {
        $$.push_back(std::move($head));
    }
    | expression_list[prev] COMMA expression[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

function_call_expression
    : name OPEN_PAREN expression_list[arguments] CLOSE_PAREN {
        $$ = std::make_unique<dtl::ast::FunctionCallExpression>();
        $$->name = std::move($name);
        $$->arguments = std::move($arguments);
    }
    ;

add_expression
    : expression[left] PLUS expression[right] {
        $$ = std::make_unique<dtl::ast::AddExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

subtract_expression
    : expression[left] MINUS expression[right] {
        $$ = std::make_unique<dtl::ast::SubtractExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

multiply_expression
    : expression[left] STAR expression[right] {
        $$ = std::make_unique<dtl::ast::MultiplyExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

divide_expression
    : expression[left] SLASH expression[right] {
        $$ = std::make_unique<dtl::ast::DivideExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

table_name
    : name {
        $$ = std::make_unique<dtl::ast::TableName>();
        $$->table_name = std::move($name);
    }
    ;

distinct_clause
    : DISTINCT {
        $$ = std::make_unique<dtl::ast::DistinctClause>();
        $$->consecutive = false;
    }
    | DISTINCT CONSECUTIVE {
        $$ = std::make_unique<dtl::ast::DistinctClause>();
        $$->consecutive = true;
    }
    | %empty {}
    ;

column_binding
    : wildcard_column_binding {
        $$ = std::move($1);
    }
    | implicit_column_binding {
        $$ = std::move($1);
    }
    | aliased_column_binding {
        $$ = std::move($1);
    }
    ;

wildcard_column_binding
    : STAR {
        $$ = std::make_unique<dtl::ast::WildcardColumnBinding>();
    }
    ;

implicit_column_binding
    : expression {
        $$ = std::make_unique<dtl::ast::ImplicitColumnBinding>();
        $$->expression = std::move($expression);
    }
    ;

aliased_column_binding
    : expression AS name[alias] {
        $$ = std::make_unique<dtl::ast::AliasedColumnBinding>();
        $$->expression = std::move($expression);
        $$->alias = std::move($alias);
    }
    ;

column_binding_list
    : column_binding[head] {
        $$.push_back(std::move($head));
    }
    | column_binding_list[prev] COMMA column_binding[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

table_binding
    : implicit_table_binding {
        $$ = std::move($1);
    }
    | aliased_table_binding {
        $$ = std::move($1);
    }
    ;

implicit_table_binding
    : OPEN_PAREN table_expression CLOSE_PAREN {
        $$ = std::make_unique<dtl::ast::ImplicitTableBinding>();
        $$->expression = std::move($table_expression);
    }
    | table_reference_expression {
        $$ = std::make_unique<dtl::ast::ImplicitTableBinding>();
        $$->expression = std::move($table_reference_expression);
    }
    ;

aliased_table_binding
    : OPEN_PAREN table_expression CLOSE_PAREN AS name {
        $$ = std::make_unique<dtl::ast::AliasedTableBinding>();
        $$->expression = std::move($table_expression);
        $$->alias = std::move($name);
    }
    | table_reference_expression AS name {
        $$ = std::make_unique<dtl::ast::AliasedTableBinding>();
        $$->expression = std::move($table_reference_expression);
        $$->alias = std::move($name);
    }
    ;

from_clause
    : FROM table_binding {
        $$ = std::make_unique<dtl::ast::FromClause>();
        $$->binding = std::move($table_binding);
    }
    ;

join_constraint
    : join_on_constraint {
        $$ = std::move($1);
    }
    | join_using_constraint {
        $$ = std::move($1);
    }
    ;

join_on_constraint
    : ON expression[predicate] {
        $$ = std::make_unique<dtl::ast::JoinOnConstraint>();
        $$->predicate = std::move($predicate);
    }
    ;

unqualified_column_name_list
    : unqualified_column_name[head] {
        $$.push_back(std::move($head));
    }
    | unqualified_column_name_list[prev] COMMA unqualified_column_name[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

join_using_constraint
    : USING OPEN_PAREN unqualified_column_name_list CLOSE_PAREN {
        $$ = std::make_unique<dtl::ast::JoinUsingConstraint>();
        $$->columns = std::move($unqualified_column_name_list);
    }
    ;

join_clause
    : JOIN table_binding join_constraint {
        $$ = std::make_unique<dtl::ast::JoinClause>();
        $$->binding = std::move($table_binding);
        $$->constraint = std::move($join_constraint);
    }
    ;

join_clause_list
    : %empty {}
    | join_clause_list[prev] join_clause[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

where_clause
    : WHERE expression {
        $$ = std::make_unique<dtl::ast::WhereClause>();
        $$->predicate = std::move($expression);
    }
    | %empty {}
    ;

group_by_clause
    : GROUP BY expression_list[pattern] {
        $$ = std::make_unique<dtl::ast::GroupByClause>();

        $$->pattern = std::move($pattern);
        $$->consecutive = false;
    }
    | GROUP CONSECUTIVE BY expression_list[pattern] {
        $$ = std::make_unique<dtl::ast::GroupByClause>();

        $$->pattern = std::move($pattern);
        $$->consecutive = true;
    }
    | %empty {}
    ;

table_expression
    : select_expression {
        $$ = std::move($1);
    }
    | import_expression {
        $$ = std::move($1);
    }
    | table_reference_expression {
        $$ = std::move($1);
    }
    ;

select_expression
    : SELECT
        distinct_clause[distinct]
        column_binding_list[columns]
        from_clause[source]
        join_clause_list[joins]
        where_clause[where]
        group_by_clause[group_by] {
            $$ = std::make_unique<dtl::ast::SelectExpression>();

            $$->distinct = std::move($distinct);
            $$->columns = std::move($columns);
            $$->source = std::move($source);
            $$->joins = std::move($joins);
            $$->where = std::move($where);
            $$->group_by = std::move($group_by);
        }
    ;

import_expression
    : IMPORT string {
        $$ = std::make_unique<dtl::ast::ImportExpression>();
        $$->location = std::move($string);
    }
    ;

table_reference_expression
    : name {
        $$ = std::make_unique<dtl::ast::TableReferenceExpression>();
        $$->name = std::move($name);
    }
    ;

statement
    : assignment_statement {
        $$ = std::move($1);
    }
//   | update_statement {
//        $$ = std::move($1);
//    }
//   | delete_statement {
//        $$ = std::move($1);
//    }
//    | insert_statement {
//        $$ = std::move($1);
//    }
    | export_statement {
        $$ = std::move($1);
    }
    ;

assignment_statement
    : name EQ table_expression SEMICOLON {
        $$ = std::make_unique<dtl::ast::AssignmentStatement>();
        $$->target = std::move($name);
        $$->expression = std::move($table_expression);
    }
    ;

export_statement
    : EXPORT table_expression TO string SEMICOLON {
        $$ = std::make_unique<dtl::ast::ExportStatement>();
        $$->location = std::move($string);
        $$->expression = std::move($table_expression);
    }
    ;

statement_list
    : %empty {}
    | statement_list[prev] statement[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

script
    : statement_list {
        $$ = std::make_unique<dtl::ast::Script>();
        $$->statements = std::move($statement_list);
    }
    ;

%%
