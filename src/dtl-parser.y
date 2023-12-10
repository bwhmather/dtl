%skeleton "lalr1.cc"
%require "3.8.1"

%define api.value.type variant

%define api.parser.class {StateMachine}
%define api.namespace {dtl::parser}

%define api.token.raw
%define api.token.constructor

%define parse.assert

%locations
%define api.location.file none

%code requires {
    #include <memory>
    #include <locale>
    #include <vector>
    #include <iostream>

    #include "dtl-ast.hpp"
    #include "dtl-tokenizer.hpp"
    #include "dtl-tokens.hpp"
}

%token <std::string_view> INT
%token <std::string_view> FLOAT
%token <std::string_view> STRING
%token <std::string_view> BYTE_STRING

/* Keywords. */
%token <std::string_view> UPDATE
%token <std::string_view> SELECT
%token <std::string_view> DISTINCT
%token <std::string_view> CONSECUTIVE
%token <std::string_view> AS
%token <std::string_view> FROM
%token <std::string_view> JOIN
%token <std::string_view> ON
%token <std::string_view> USING
%token <std::string_view> WHERE
%token <std::string_view> GROUP
%token <std::string_view> BY
%token <std::string_view> WITH
%token <std::string_view> IMPORT
%token <std::string_view> EXPORT
%token <std::string_view> TO

/* Symbols */
%token <std::string_view> SEMICOLON
%token <std::string_view> COMMA
%token <std::string_view> DOT
%token <std::string_view> OPEN_PAREN
%token <std::string_view> CLOSE_PAREN
%token <std::string_view> OPEN_BRACE
%token <std::string_view> CLOSE_BRACE
%token <std::string_view> OPEN_BRACKET
%token <std::string_view> CLOSE_BRACKET
%token <std::string_view> AT
%token <std::string_view> POUND
%token <std::string_view> TILDE
%token <std::string_view> QUESTION
%token <std::string_view> COLON
%token <std::string_view> DOLLAR

/* Operators */
%token <std::string_view> EQ
%token <std::string_view> NOT
%token <std::string_view> NOT_EQUAL
%token <std::string_view> LESS_THAN
%token <std::string_view> LESS_THAN_EQUAL
%token <std::string_view> GREATER_THAN
%token <std::string_view> GREATER_THAN_EQUAL
%token <std::string_view> MINUS
%token <std::string_view> PLUS
%token <std::string_view> STAR
%token <std::string_view> SLASH
%token <std::string_view> CARET
%token <std::string_view> PERCENT

/* Identifiers */
%token <std::string_view> TYPE
%token <std::string_view> NAME
%token <std::string_view> QUOTED_NAME

/* Blanks */
%token <std::string_view> LINE_COMMENT
%token <std::string_view> BLOCK_COMMENT
%token <std::string_view> WHITESPACE

%param {dtl::tokenizer::Tokenizer& tokenizer}
%parse-param {std::unique_ptr<const dtl::ast::Script>& result}

%code {
    static dtl::parser::StateMachine::symbol_type
    yylex(dtl::tokenizer::Tokenizer& tokenizer) {
        using sm = dtl::parser::StateMachine;

        while (true) {
            auto token = tokenizer.next_token();

            /* Map tokenizer location to parser location. */
            dtl::parser::position token_start(
                nullptr, token.start.lineno, token.start.column
            );
            dtl::parser::position token_end(
                nullptr, token.end.lineno, token.end.column
            );
            dtl::parser::location token_location(token_start, token_end);

            /* Map tokenizer start and end pointers to parser string view. */
            // TODO is should be able to construct a string view from two
            // iterators in c++20, but constructor not found.
            std::string_view token_value(
                token.start.offset.operator->(),
                token.end.offset - token.start.offset
            );

            /* Map tokenizer type to parser token type. */
            switch (token.type) {
            case dtl::tokens::Int:
                return sm::make_INT(token_value, token_location);
            case dtl::tokens::Float:
                return sm::make_FLOAT(token_value, token_location);
            case dtl::tokens::String:
                return sm::make_STRING(token_value, token_location);
            case dtl::tokens::ByteString:
                return sm::make_BYTE_STRING(token_value, token_location);

            /* Keywords. */
            case dtl::tokens::As:
                return sm::make_AS(token_value, token_location);
            case dtl::tokens::By:
                return sm::make_BY(token_value, token_location);
            case dtl::tokens::Consecutive:
                return sm::make_CONSECUTIVE(token_value, token_location);
            case dtl::tokens::Distinct:
                return sm::make_DISTINCT(token_value, token_location);
            case dtl::tokens::Export:
                return sm::make_EXPORT(token_value, token_location);
            case dtl::tokens::From:
                return sm::make_FROM(token_value, token_location);
            case dtl::tokens::Group:
                return sm::make_GROUP(token_value, token_location);
            case dtl::tokens::Import:
                return sm::make_IMPORT(token_value, token_location);
            case dtl::tokens::Join:
                return sm::make_JOIN(token_value, token_location);
            case dtl::tokens::On:
                return sm::make_ON(token_value, token_location);
            case dtl::tokens::Select:
                return sm::make_SELECT(token_value, token_location);
            case dtl::tokens::To:
                return sm::make_TO(token_value, token_location);
            case dtl::tokens::Update:
                return sm::make_UPDATE(token_value, token_location);
            case dtl::tokens::Using:
                return sm::make_USING(token_value, token_location);
            case dtl::tokens::Where:
                return sm::make_WHERE(token_value, token_location);
            case dtl::tokens::With:
                return sm::make_WITH(token_value, token_location);

            /* Symbols */
            case dtl::tokens::Semicolon:
                return sm::make_SEMICOLON(token_value, token_location);
            case dtl::tokens::Comma:
                return sm::make_COMMA(token_value, token_location);
            case dtl::tokens::Dot:
                return sm::make_DOT(token_value, token_location);
            case dtl::tokens::OpenParen:
                return sm::make_OPEN_PAREN(token_value, token_location);
            case dtl::tokens::CloseParen:
                return sm::make_CLOSE_PAREN(token_value, token_location);
            case dtl::tokens::OpenBrace:
                return sm::make_OPEN_BRACE(token_value, token_location);
            case dtl::tokens::CloseBrace:
                return sm::make_CLOSE_BRACE(token_value, token_location);
            case dtl::tokens::OpenBracket:
                return sm::make_OPEN_BRACKET(token_value, token_location);
            case dtl::tokens::CloseBracket:
                return sm::make_CLOSE_BRACKET(token_value, token_location);
            case dtl::tokens::At:
                return sm::make_AT(token_value, token_location);
            case dtl::tokens::Pound:
                return sm::make_POUND(token_value, token_location);
            case dtl::tokens::Tilde:
                return sm::make_TILDE(token_value, token_location);
            case dtl::tokens::Question:
                return sm::make_QUESTION(token_value, token_location);
            case dtl::tokens::Colon:
                return sm::make_COLON(token_value, token_location);
            case dtl::tokens::Dollar:
                return sm::make_DOLLAR(token_value, token_location);

            /* Operators */
            case dtl::tokens::Eq:
                return sm::make_EQ(token_value, token_location);
            case dtl::tokens::Not:
                return sm::make_NOT(token_value, token_location);
            case dtl::tokens::NotEqual:
                return sm::make_NOT_EQUAL(token_value, token_location);
            case dtl::tokens::LessThan:
                return sm::make_LESS_THAN(token_value, token_location);
            case dtl::tokens::LessThanEqual:
                return sm::make_LESS_THAN_EQUAL(token_value, token_location);
            case dtl::tokens::GreaterThan:
                return sm::make_GREATER_THAN(token_value, token_location);
            case dtl::tokens::GreaterThanEqual:
                return sm::make_GREATER_THAN_EQUAL(token_value, token_location);
            case dtl::tokens::Minus:
                return sm::make_MINUS(token_value, token_location);
            case dtl::tokens::Plus:
                return sm::make_PLUS(token_value, token_location);
            case dtl::tokens::Star:
                return sm::make_STAR(token_value, token_location);
            case dtl::tokens::Slash:
                return sm::make_SLASH(token_value, token_location);
            case dtl::tokens::Caret:
                return sm::make_CARET(token_value, token_location);
            case dtl::tokens::Percent:
                return sm::make_PERCENT(token_value, token_location);

            /* Identifiers */
            case dtl::tokens::Type:
                return sm::make_TYPE(token_value, token_location);
            case dtl::tokens::Name:
                return sm::make_NAME(token_value, token_location);
            case dtl::tokens::QuotedName:
                return sm::make_QUOTED_NAME(token_value, token_location);

            /* Blanks */
            case dtl::tokens::LineComment:
                continue;
            case dtl::tokens::BlockComment:
                continue;
            case dtl::tokens::Whitespace:
                continue;

            /* Special */
            case dtl::tokens::EndOfFile:
                return sm::make_YYEOF(token_location);
            case dtl::tokens::Error:
                return sm::make_YYUNDEF(token_location);

            default:
                assert(false);
            }
        }
    }
}

%left PLUS MINUS
%left STAR SLASH

%type <std::string> name;

%type <dtl::unique_variant_ptr_t<const dtl::ast::Literal>> literal;
%type <std::unique_ptr<const dtl::ast::String>> string;

%type <dtl::unique_variant_ptr_t<const dtl::ast::ColumnName>> column_name;
%type <std::vector<std::unique_ptr<const dtl::ast::UnqualifiedColumnName>>> unqualified_column_name_list;
%type <std::unique_ptr<const dtl::ast::UnqualifiedColumnName>> unqualified_column_name;
%type <std::unique_ptr<const dtl::ast::QualifiedColumnName>> qualified_column_name;

%type <dtl::unique_variant_ptr_t<const dtl::ast::Expression>> expression;
%type <std::unique_ptr<const dtl::ast::ColumnReferenceExpression>> column_reference_expression;
%type <std::unique_ptr<const dtl::ast::LiteralExpression>> literal_expression;
%type <std::vector<dtl::unique_variant_ptr_t<const dtl::ast::Expression>>> expression_list;
%type <std::unique_ptr<const dtl::ast::FunctionCallExpression>> function_call_expression;
%type <std::unique_ptr<const dtl::ast::AddExpression>> add_expression;
%type <std::unique_ptr<const dtl::ast::SubtractExpression>> subtract_expression;
%type <std::unique_ptr<const dtl::ast::MultiplyExpression>> multiply_expression;
%type <std::unique_ptr<const dtl::ast::DivideExpression>> divide_expression;

%type <std::unique_ptr<const dtl::ast::TableName>> table_name;
%type <std::unique_ptr<const dtl::ast::DistinctClause>> distinct_clause;

%type <std::unique_ptr<const dtl::ast::ColumnBinding>> column_binding;
%type <std::vector<std::unique_ptr<const dtl::ast::ColumnBinding>>> column_binding_list;
%type <std::unique_ptr<const dtl::ast::WildcardColumnBinding>> wildcard_column_binding;
%type <std::unique_ptr<const dtl::ast::ImplicitColumnBinding>> implicit_column_binding;
%type <std::unique_ptr<const dtl::ast::AliasedColumnBinding>> aliased_column_binding;

%type <std::unique_ptr<const dtl::ast::TableBinding>> table_binding;
%type <std::unique_ptr<const dtl::ast::ImplicitTableBinding>> implicit_table_binding;
%type <std::unique_ptr<const dtl::ast::AliasedTableBinding>> aliased_table_binding;

%type <std::unique_ptr<const dtl::ast::FromClause>> from_clause;

%type <std::unique_ptr<const dtl::ast::JoinConstraint>> join_constraint;
%type <std::unique_ptr<const dtl::ast::JoinOnConstraint>> join_on_constraint;
%type <std::unique_ptr<const dtl::ast::JoinUsingConstraint>> join_using_constraint;

%type <std::unique_ptr<const dtl::ast::JoinClause>> join_clause;
%type <std::vector<std::unique_ptr<const dtl::ast::JoinClause>>> join_clause_list;

%type <std::unique_ptr<const dtl::ast::WhereClause>> where_clause;
%type <std::unique_ptr<const dtl::ast::GroupByClause>> group_by_clause;

%type <std::unique_ptr<const dtl::ast::TableExpression>> table_expression;
%type <std::unique_ptr<const dtl::ast::SelectExpression>> select_expression;
%type <std::unique_ptr<const dtl::ast::ImportExpression>> import_expression;
%type <std::unique_ptr<const dtl::ast::TableReferenceExpression>> table_reference_expression;

%type <std::unique_ptr<const dtl::ast::Statement>> statement;
%type <std::vector<std::unique_ptr<const dtl::ast::Statement>>> statement_list;
%type <std::unique_ptr<const dtl::ast::AssignmentStatement>> assignment_statement;
//%type <std::unique_ptr<const dtl::ast::UpdateStatement>> update_statement;
//%type <std::unique_ptr<const dtl::ast::DeleteStatement>> delete_statement;
//%type <std::unique_ptr<const dtl::ast::InsertStatement>> insert_statement;
%type <std::unique_ptr<const dtl::ast::ExportStatement>> export_statement;

%type <std::unique_ptr<const dtl::ast::Script>> script;
%type result;

%%
%start result;

name
    : QUOTED_NAME {
        // TODO unquote.
        $$ = std::string($1);
    }
    | NAME {
        $$ = std::string($1);
    }
    ;

literal
    : string {
        $$.emplace<std::unique_ptr<const dtl::ast::String>>();
        std::get<std::unique_ptr<const dtl::ast::String>>($$).swap($1);
    }
    ;

string
    : STRING {
        auto expr = std::make_unique<dtl::ast::String>();
        expr->value = std::string();
        auto curr = $1.begin();
        while (true) {
            curr++;
            if (curr == $1.end()) {
                throw "unexpected end of string";
            }
            if (*curr == '\'') {
                curr++;
                if (curr == $1.end()) {
                    break;
                }

                if (*curr != '\'') {
                    throw "unexpected single quote";
                }
            }
            expr->value += *curr;
        }
        $$ = std::move(expr);
    }
    ;

column_name
    : unqualified_column_name {
        $$.emplace<std::unique_ptr<const dtl::ast::UnqualifiedColumnName>>();
        std::get<std::unique_ptr<const dtl::ast::UnqualifiedColumnName>>($$).swap($1);
    }
    | qualified_column_name {
        $$.emplace<std::unique_ptr<const dtl::ast::QualifiedColumnName>>();
        std::get<std::unique_ptr<const dtl::ast::QualifiedColumnName>>($$).swap($1);
    }
    ;

unqualified_column_name
    : name[column_name] {
        auto expr = std::make_unique<dtl::ast::UnqualifiedColumnName>();
        expr->column_name = std::move($column_name);
        $$ = std::move(expr);
    }
    ;

qualified_column_name
    : name[table_name] DOT name[column_name] {
        auto expr = std::make_unique<dtl::ast::QualifiedColumnName>();
        expr->table_name = std::move($table_name);
        expr->column_name = std::move($column_name);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::ColumnReferenceExpression>();
        expr->name = std::move($name);
        $$ = std::move(expr);
    }
    ;

literal_expression
    : literal[value] {
        auto expr = std::make_unique<dtl::ast::LiteralExpression>();
        expr->value = std::move($value);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::FunctionCallExpression>();
        expr->name = std::move($name);
        expr->arguments = std::move($arguments);
        $$ = std::move(expr);
    }
    ;

add_expression
    : expression[left] PLUS expression[right] {
        auto expr = std::make_unique<dtl::ast::AddExpression>();
        expr->left = std::move($left);
        expr->right = std::move($right);
        $$ = std::move(expr);
    }
    ;

subtract_expression
    : expression[left] MINUS expression[right] {
        auto expr = std::make_unique<dtl::ast::SubtractExpression>();
        expr->left = std::move($left);
        expr->right = std::move($right);
        $$ = std::move(expr);
    }
    ;

multiply_expression
    : expression[left] STAR expression[right] {
        auto expr = std::make_unique<dtl::ast::MultiplyExpression>();
        expr->left = std::move($left);
        expr->right = std::move($right);
        $$ = std::move(expr);
    }
    ;

divide_expression
    : expression[left] SLASH expression[right] {
        auto expr = std::make_unique<dtl::ast::DivideExpression>();
        expr->left = std::move($left);
        expr->right = std::move($right);
        $$ = std::move(expr);
    }
    ;

table_name
    : name {
        auto expr = std::make_unique<dtl::ast::TableName>();
        expr->table_name = std::move($name);
        $$ = std::move(expr);
    }
    ;

distinct_clause
    : DISTINCT {
        auto expr = std::make_unique<dtl::ast::DistinctClause>();
        expr->consecutive = false;
        $$ = std::move(expr);
    }
    | DISTINCT CONSECUTIVE {
        auto expr = std::make_unique<dtl::ast::DistinctClause>();
        expr->consecutive = true;
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::WildcardColumnBinding>();
        $$ = std::move(expr);
    }
    ;

implicit_column_binding
    : expression {
        auto expr = std::make_unique<dtl::ast::ImplicitColumnBinding>();
        expr->expression = std::move($expression);
        $$ = std::move(expr);
    }
    ;

aliased_column_binding
    : expression AS name[alias] {
        auto expr = std::make_unique<dtl::ast::AliasedColumnBinding>();
        expr->expression = std::move($expression);
        expr->alias = std::move($alias);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::ImplicitTableBinding>();
        expr->expression = std::move($table_expression);
        $$ = std::move(expr);
    }
    | table_reference_expression {
        auto expr = std::make_unique<dtl::ast::ImplicitTableBinding>();
        expr->expression = std::move($table_reference_expression);
        $$ = std::move(expr);
    }
    ;

aliased_table_binding
    : OPEN_PAREN table_expression CLOSE_PAREN AS name {
        auto expr = std::make_unique<dtl::ast::AliasedTableBinding>();
        expr->expression = std::move($table_expression);
        expr->alias = std::move($name);
        $$ = std::move(expr);
    }
    | table_reference_expression AS name {
        auto expr = std::make_unique<dtl::ast::AliasedTableBinding>();
        expr->expression = std::move($table_reference_expression);
        expr->alias = std::move($name);
        $$ = std::move(expr);
    }
    ;

from_clause
    : FROM table_binding {
        auto expr = std::make_unique<dtl::ast::FromClause>();
        expr->binding = std::move($table_binding);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::JoinOnConstraint>();
        expr->predicate = std::move($predicate);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::JoinUsingConstraint>();
        expr->columns = std::move($unqualified_column_name_list);
        $$ = std::move(expr);
    }
    ;

join_clause
    : JOIN table_binding join_constraint {
        auto expr = std::make_unique<dtl::ast::JoinClause>();
        expr->binding = std::move($table_binding);
        expr->constraint = std::move($join_constraint);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::WhereClause>();
        expr->predicate = std::move($expression);
        $$ = std::move(expr);
    }
    | %empty {}
    ;

group_by_clause
    : GROUP BY expression_list[pattern] {
        auto expr = std::make_unique<dtl::ast::GroupByClause>();

        expr->pattern = std::move($pattern);
        expr->consecutive = false;
        $$ = std::move(expr);
    }
    | GROUP CONSECUTIVE BY expression_list[pattern] {
        auto expr = std::make_unique<dtl::ast::GroupByClause>();

        expr->pattern = std::move($pattern);
        expr->consecutive = true;
        $$ = std::move(expr);
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
            auto expr = std::make_unique<dtl::ast::SelectExpression>();

            expr->distinct = std::move($distinct);
            expr->columns = std::move($columns);
            expr->source = std::move($source);
            expr->joins = std::move($joins);
            expr->where = std::move($where);
            expr->group_by = std::move($group_by);

            $$ = std::move(expr);
        }
    ;

import_expression
    : IMPORT string {
        auto expr = std::make_unique<dtl::ast::ImportExpression>();
        expr->location = std::move($string);
        $$ = std::move(expr);
    }
    ;

table_reference_expression
    : name {
        auto expr = std::make_unique<dtl::ast::TableReferenceExpression>();
        expr->name = std::move($name);
        $$ = std::move(expr);
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
    : WITH table_name AS table_expression SEMICOLON {
        auto expr = std::make_unique<dtl::ast::AssignmentStatement>();
        expr->target = std::move($table_name);
        expr->expression = std::move($table_expression);
        $$ = std::move(expr);
    }
    ;

export_statement
    : EXPORT table_expression TO string SEMICOLON {
        auto expr = std::make_unique<dtl::ast::ExportStatement>();
        expr->location = std::move($string);
        expr->expression = std::move($table_expression);
        $$ = std::move(expr);
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
        auto expr = std::make_unique<dtl::ast::Script>();
        expr->statements = std::move($statement_list);
        $$ = std::move(expr);
    }
    ;

result
    : script {
        result = std::move($script);
    }
    ;

%%

void dtl::parser::StateMachine::error(
    const location_type& location, const std::string& message
) {
    std::cerr << location << ": " << message << '\n';
}

namespace dtl {
namespace parser {

std::unique_ptr<const dtl::ast::Script> parse(dtl::tokenizer::Tokenizer& tokenizer) {
    std::unique_ptr<const dtl::ast::Script> result;
    dtl::parser::StateMachine state_machine(tokenizer, result);
    assert(!state_machine.parse());
    return result;
}

}  /* namespace parser */
}  /* namespace dtl */

