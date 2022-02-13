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
%token <std::string> Name
%token <std::string> QuotedName

    /* Blanks */
%token LineComment
%token BlockComment
%token Whitespace


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
    : QuotedName
    | Name
    ;

literal
    : string { $$ = std::move($1); }
    ;

string
    : String {

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
    : name[table_name] Dot name[column_name] {
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
    | expression_list[prev] Comma expression[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

function_call_expression
    : name OpenParen expression_list[arguments] CloseParen {
        $$ = std::make_unique<dtl::ast::FunctionCallExpression>();
        $$->name = std::move($name);
        $$->arguments = std::move($arguments);
    }
    ;

add_expression
    : expression[left] Plus expression[right] {
        $$ = std::make_unique<dtl::ast::AddExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

subtract_expression
    : expression[left] Minus expression[right] {
        $$ = std::make_unique<dtl::ast::SubtractExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

multiply_expression
    : expression[left] Star expression[right] {
        $$ = std::make_unique<dtl::ast::MultiplyExpression>();
        $$->left = std::move($left);
        $$->right = std::move($right);
    }
    ;

divide_expression
    : expression[left] Slash expression[right] {
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
    : Distinct {
        $$ = std::make_unique<dtl::ast::DistinctClause>();
        $$->consecutive = false;
    }
    | Distinct Consecutive {
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
    : Star {
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
    : expression As name[alias] {
        $$ = std::make_unique<dtl::ast::AliasedColumnBinding>();
        $$->expression = std::move($expression);
        $$->alias = std::move($alias);
    }
    ;

column_binding_list
    : column_binding[head] {
        $$.push_back(std::move($head));
    }
    | column_binding_list[prev] Comma column_binding[next] {
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
    : OpenParen table_expression CloseParen {
        $$ = std::make_unique<dtl::ast::ImplicitTableBinding>();
        $$->expression = std::move($table_expression);
    }
    | table_reference_expression {
        $$ = std::make_unique<dtl::ast::ImplicitTableBinding>();
        $$->expression = std::move($table_reference_expression);
    }
    ;

aliased_table_binding
    : OpenParen table_expression CloseParen As name {
        $$ = std::make_unique<dtl::ast::AliasedTableBinding>();
        $$->expression = std::move($table_expression);
        $$->alias = std::move($name);
    }
    | table_reference_expression As name {
        $$ = std::make_unique<dtl::ast::AliasedTableBinding>();
        $$->expression = std::move($table_reference_expression);
        $$->alias = std::move($name);
    }
    ;

from_clause
    : From table_binding {
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
    : On expression[predicate] {
        $$ = std::make_unique<dtl::ast::JoinOnConstraint>();
        $$->predicate = std::move($predicate);
    }
    ;

unqualified_column_name_list
    : unqualified_column_name[head] {
        $$.push_back(std::move($head));
    }
    | unqualified_column_name_list[prev] Comma unqualified_column_name[next] {
        $$ = std::move($prev);
        $$.push_back(std::move($next));
    }
    ;

join_using_constraint
    : Using OpenParen unqualified_column_name_list CloseParen {
        $$ = std::make_unique<dtl::ast::JoinUsingConstraint>();
        $$->columns = std::move($unqualified_column_name_list);
    }
    ;

join_clause
    : Join table_binding join_constraint {
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
    : Where expression {
        $$ = std::make_unique<dtl::ast::WhereClause>();
        $$->predicate = std::move($expression);
    }
    | %empty {}
    ;

group_by_clause
    : Group By expression_list[pattern] {
        $$ = std::make_unique<dtl::ast::GroupByClause>();

        $$->pattern = std::move($pattern);
        $$->consecutive = false;
    }
    | Group Consecutive By expression_list[pattern] {
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
    : Select
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
    : Import string {
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
    : name Eq table_expression Semicolon {
        $$ = std::make_unique<dtl::ast::AssignmentStatement>();
        $$->target = std::move($name);
        $$->expression = std::move($table_expression);
    }
    ;

export_statement
    : Export table_expression To string Semicolon {
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
