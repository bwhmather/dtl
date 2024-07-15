%define api.pure true
%define api.prefix {dtl_parser_}
%locations

%define parse.error verbose

%code top {
    #include "dtl-tokens.h"
}

%code requires {
    #include "dtl-parser.h"

    #include <stdbool.h>
    #include <stdlib.h>

    #include "dtl-ast.h"
    #include "dtl-ast-node.h"
    #include "dtl-tokenizer.h"
}

%union {
    struct dtl_token token;
    struct dtl_ast_node *node;
}

%param {struct dtl_tokenizer *tokenizer}
%parse-param {struct dtl_ast_node **result}

%token <token> INT
%token <token> FLOAT
%token <token> STRING
%token <token> BYTE_STRING

/* Keywords. */
%token <token> UPDATE
%token <token> SELECT
%token <token> DISTINCT
%token <token> CONSECUTIVE
%token <token> AS
%token <token> FROM
%token <token> JOIN
%token <token> ON
%token <token> USING
%token <token> WHERE
%token <token> GROUP
%token <token> BY
%token <token> WITH
%token <token> IMPORT
%token <token> EXPORT
%token <token> TO

/* Symbols */
%token <token> SEMICOLON
%token <token> COMMA
%token <token> DOT
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> AT
%token <token> POUND
%token <token> TILDE
%token <token> QUESTION
%token <token> COLON
%token <token> DOLLAR

/* Operators */
%token <token> EQUAL
%token <token> NOT
%token <token> NOT_EQUAL
%token <token> LESS_THAN
%token <token> LESS_THAN_OR_EQUAL
%token <token> GREATER_THAN
%token <token> GREATER_THAN_OR_EQUAL
%token <token> MINUS
%token <token> PLUS
%token <token> STAR
%token <token> SLASH
%token <token> CARET
%token <token> PERCENT

/* Identifiers */
%token <token> TYPE
%token <token> NAME
%token <token> QUOTED_NAME

/* Blanks */
%token <token> LINE_COMMENT
%token <token> BLOCK_COMMENT
%token <token> WHITESPACE

%code {
    static int
    yylex(YYSTYPE *lvalp, YYLTYPE *llocp, struct dtl_tokenizer *tokenizer) {
        (void) lvalp;
        (void) tokenizer;

        while (true) {
            struct dtl_token next = dtl_tokenizer_next_token(tokenizer);

            lvalp->token = next;
            llocp->first_line = next.start.lineno;
            llocp->first_column = next.start.column;
            llocp->last_line = next.start.lineno;
            llocp->last_column = next.start.column;

            switch (next.type) {
            case DTL_TOKEN_END_OF_FILE:
                return DTL_PARSER_EOF;
            case DTL_TOKEN_ERROR:
                return DTL_PARSER_UNDEF;

            /* LITERALS. */
            case DTL_TOKEN_INT:
                return INT;
            case DTL_TOKEN_FLOAT:
                return FLOAT;
            case DTL_TOKEN_STRING:
                return STRING;
            case DTL_TOKEN_BYTE_STRING:
                return BYTE_STRING;

          /* KEYWORDS. */
            case DTL_TOKEN_AS:
                return AS;
            case DTL_TOKEN_BY:
                return BY;
            case DTL_TOKEN_CONSECUTIVE:
                return CONSECUTIVE;
            case DTL_TOKEN_DISTINCT:
                return DISTINCT;
            case DTL_TOKEN_EXPORT:
                return EXPORT;
            case DTL_TOKEN_FROM:
                return FROM;
            case DTL_TOKEN_GROUP:
                return GROUP;
            case DTL_TOKEN_IMPORT:
                return IMPORT;
            case DTL_TOKEN_JOIN:
                return JOIN;
            case DTL_TOKEN_ON:
                return ON;
            case DTL_TOKEN_SELECT:
                return SELECT;
            case DTL_TOKEN_TO:
                return TO;
            case DTL_TOKEN_UPDATE:
                return UPDATE;
            case DTL_TOKEN_USING:
                return USING;
            case DTL_TOKEN_WHERE:
                return WHERE;
            case DTL_TOKEN_WITH:
                return WITH;

            /* SYMBOLS */
            case DTL_TOKEN_SEMICOLON:
                return SEMICOLON;
            case DTL_TOKEN_COMMA:
                return COMMA;
            case DTL_TOKEN_DOT:
                return DOT;
            case DTL_TOKEN_OPEN_PARENTHESIS:
                return OPEN_PARENTHESIS;
            case DTL_TOKEN_CLOSE_PARENTHESIS:
                return CLOSE_PARENTHESIS;
            case DTL_TOKEN_OPEN_BRACE:
                return OPEN_BRACE;
            case DTL_TOKEN_CLOSE_BRACE:
                return CLOSE_BRACE;
            case DTL_TOKEN_OPEN_BRACKET:
                return OPEN_BRACKET;
            case DTL_TOKEN_CLOSE_BRACKET:
                return CLOSE_BRACKET;
            case DTL_TOKEN_AT:
                return AT;
            case DTL_TOKEN_POUND:
                return POUND;
            case DTL_TOKEN_TILDE:
                return TILDE;
            case DTL_TOKEN_QUESTION:
                return QUESTION;
            case DTL_TOKEN_COLON:
                return COLON;
            case DTL_TOKEN_DOLLAR:
                return DOLLAR;

            /* OPERATORS */
            case DTL_TOKEN_EQUAL:
                return EQUAL;
            case DTL_TOKEN_NOT:
                return NOT;
            case DTL_TOKEN_NOT_EQUAL:
                return NOT_EQUAL;
            case DTL_TOKEN_LESS_THAN:
                return LESS_THAN;
            case DTL_TOKEN_LESS_THAN_OR_EQUAL:
                return LESS_THAN_OR_EQUAL;
            case DTL_TOKEN_GREATER_THAN:
                return GREATER_THAN;
            case DTL_TOKEN_GREATER_THAN_OR_EQUAL:
                return GREATER_THAN_OR_EQUAL;
            case DTL_TOKEN_MINUS:
                return MINUS;
            case DTL_TOKEN_PLUS:
                return PLUS;
            case DTL_TOKEN_STAR:
                return STAR;
            case DTL_TOKEN_SLASH:
                return SLASH;
            case DTL_TOKEN_CARET:
                return CARET;
            case DTL_TOKEN_PERCENT:
                return PERCENT;

            /* IDENTIFIERS */
            case DTL_TOKEN_TYPE:
                return TYPE;
            case DTL_TOKEN_NAME:
                return NAME;
            case DTL_TOKEN_QUOTED_NAME:
                return QUOTED_NAME;

            /* BLANKS */
            case DTL_TOKEN_LINE_COMMENT:
                continue;
            case DTL_TOKEN_BLOCK_COMMENT:
                continue;
            case DTL_TOKEN_WHITESPACE:
                continue;
            }
        }

        return 0;
    }

    static int
    yyerror(YYLTYPE *llocp, struct dtl_tokenizer *tokenizer, struct dtl_ast_node **result, const char *s) {
        (void) llocp;
        (void) tokenizer;
        (void) result;
        (void) s;
        return 0;
    }
}

%left EQUAL NOT_EQUAL LESS_THAN LESS_THAN_OR_EQUAL GREATER_THAN GREATER_THAN_OR_EQUAL
%left PLUS MINUS
%left STAR SLASH

%type <node> name;

%type <node> literal;
%type <node> string;

%type <node> column_name;
%type <node> join_using_constraint_name_list;
%type <node> unqualified_column_name;
%type <node> qualified_column_name;

%type <node> expression;
%type <node> column_reference_expression;
%type <node> literal_expression;
%type <node> expression_list;
%type <node> function_call_expression;
%type <node> add_expression;
%type <node> subtract_expression;
%type <node> multiply_expression;
%type <node> divide_expression;

%type <node> table_name;
%type <node> distinct_clause;

%type <node> column_binding;
%type <node> column_binding_list;
%type <node> wildcard_column_binding;
%type <node> implicit_column_binding;
%type <node> aliased_column_binding;

%type <node> table_binding;
%type <node> implicit_table_binding;
%type <node> aliased_table_binding;

%type <node> from_clause;

%type <node> join_constraint;
%type <node> join_on_constraint;
%type <node> join_using_constraint;

%type <node> join_clause;
%type <node> join_clause_list;

%type <node> where_clause;
%type <node> group_by_clause;

%type <node> table_expression;
%type <node> select_expression;
%type <node> import_expression;
%type <node> table_reference_expression;

%type <node> statement;
%type <node> statement_list;
%type <node> assignment_statement;
//%type <node> update_statement;
//%type <node> delete_statement;
//%type <node> insert_statement;
%type <node> export_statement;

%type <node> script;
%type result;

%%
%start result;

name
    : QUOTED_NAME {
        $$ = dtl_ast_node_create_empty(DTL_AST_NAME);  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $1.end);

    }
    | NAME {
        $$ = dtl_ast_node_create_empty(DTL_AST_NAME);  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $1.end);
    }
    ;

literal
    : string {
        $$ = $1;
    }
    ;

string
    : STRING {
        $$ = dtl_ast_node_create_empty(DTL_AST_STRING_LITERAL);  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $1.end);
    }
    ;

column_name
    : unqualified_column_name {
        $$ = $1;
    }
    | qualified_column_name {
        $$ = $1;
    }
    ;

unqualified_column_name
    : name {
        $$ = dtl_ast_node_create_with_children(DTL_AST_UNQUALIFIED_COLUMN_NAME);
        $$ = dtl_ast_node_append_child($$, $name);
    }
    ;

qualified_column_name
    : name[table_name] DOT name[column_name] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_QUALIFIED_COLUMN_NAME);
        $$ = dtl_ast_node_append_child($$, $table_name);
        $$ = dtl_ast_node_append_child($$, $column_name);
    }
    ;

expression
    : column_reference_expression {
        $$ = $1;
    }
    | literal_expression {
        $$ = $1;
    }
    | function_call_expression {
        $$ = $1;
    }
    | add_expression {
        $$ = $1;
    }
    | subtract_expression {
        $$ = $1;
    }
    | multiply_expression {
        $$ = $1;
    }
    | divide_expression {
        $$ = $1;
    }
    ;

column_reference_expression
    : column_name[name] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_COLUMN_REFERENCE_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $name);
    }
    ;

literal_expression
    : literal[value] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_LITERAL_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $value);
    }
    ;

expression_list
    : expression[head] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_EXPRESSION_LIST);
        $$ = dtl_ast_node_append_child($$, $head);
    }
    | expression_list[prev] COMMA expression[next] {
        $$ = dtl_ast_node_append_child($prev, $next);
    }
    ;

function_call_expression
    : name OPEN_PARENTHESIS expression_list[arguments] CLOSE_PARENTHESIS {
        $$ = dtl_ast_node_create_with_children(DTL_AST_FUNCTION_CALL_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $name);
        $$ = dtl_ast_node_append_child($$, $arguments);
    }
    ;

add_expression
    : expression[left] PLUS expression[right] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_ADD_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $left);
        $$ = dtl_ast_node_append_child($$, $right);
    }
    ;

subtract_expression
    : expression[left] MINUS expression[right] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_SUBTRACT_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $left);
        $$ = dtl_ast_node_append_child($$, $right);
    }
    ;

multiply_expression
    : expression[left] STAR expression[right] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_MULTIPLY_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $left);
        $$ = dtl_ast_node_append_child($$, $right);
    }
    ;

divide_expression
    : expression[left] SLASH expression[right] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_DIVIDE_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $left);
        $$ = dtl_ast_node_append_child($$, $right);
    }
    ;

table_name
    : name {
        $$ = dtl_ast_node_create_with_children(DTL_AST_TABLE_NAME);
        $$ = dtl_ast_node_append_child($$, $name);
    }
    ;

distinct_clause
    : DISTINCT {
        $$ = dtl_ast_node_create_empty(DTL_AST_DISTINCT_CLAUSE);  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $1.end);
    }
    | DISTINCT CONSECUTIVE {
        $$ = dtl_ast_node_create_empty(DTL_AST_DISTINCT_CONSECUTIVE_CLAUSE);  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $2.end);
    }
    | %empty {
        $$ = NULL;
    }
    ;

column_binding
    : wildcard_column_binding {
        $$ = $1;
    }
    | implicit_column_binding {
        $$ = $1;
    }
    | aliased_column_binding {
        $$ = $1;
    }
    ;

wildcard_column_binding
    : STAR {
        $$ = dtl_ast_node_create_empty(DTL_AST_WILDCARD_COLUMN_BINDING);  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $1.end);
    }
    ;

implicit_column_binding
    : expression {
        $$ = dtl_ast_node_create_with_children(DTL_AST_IMPLICIT_COLUMN_BINDING);
        $$ = dtl_ast_node_append_child($$, $expression);
    }
    ;

aliased_column_binding
    : expression AS name[alias] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_ALIASED_COLUMN_BINDING);
        $$ = dtl_ast_node_append_child($$, $expression);
        $$ = dtl_ast_node_append_child($$, $alias);
    }
    ;

column_binding_list
    : column_binding[head] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_COLUMN_BINDING_LIST);
        $$ = dtl_ast_node_append_child($$, $head);
    }
    | column_binding_list[prev] COMMA column_binding[next] {
        $$ = dtl_ast_node_append_child($prev, $next);
    }
    ;

table_binding
    : implicit_table_binding {
        $$ = $1;
    }
    | aliased_table_binding {
        $$ = $1;
    }
    ;

implicit_table_binding
    : OPEN_PARENTHESIS table_expression[expression] CLOSE_PARENTHESIS {
        $$ = dtl_ast_node_create_with_children(DTL_AST_IMPLICIT_TABLE_BINDING);
        $$ = dtl_ast_node_append_child($$, $expression);
    }
    | table_reference_expression[expression] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_IMPLICIT_TABLE_BINDING);
        $$ = dtl_ast_node_append_child($$, $expression);
    }
    ;

aliased_table_binding
    : OPEN_PARENTHESIS table_expression[expression] CLOSE_PARENTHESIS AS name[alias] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_ALIASED_TABLE_BINDING);
        $$ = dtl_ast_node_append_child($$, $expression);
        $$ = dtl_ast_node_append_child($$, $alias);
    }
    | table_reference_expression[expression] AS name[alias] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_ALIASED_TABLE_BINDING);
        $$ = dtl_ast_node_append_child($$, $expression);
        $$ = dtl_ast_node_append_child($$, $alias);
    }
    ;

from_clause
    : FROM table_binding {
        $$ = dtl_ast_node_create_with_children(DTL_AST_FROM_CLAUSE);
        $$ = dtl_ast_node_append_child($$, $table_binding);
    }
    ;

join_constraint
    : join_on_constraint {
        $$ = $1;
    }
    | join_using_constraint {
        $$ = $1;
    }
    ;

join_on_constraint
    : ON expression[predicate] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_JOIN_ON_CONSTRAINT);
        $$ = dtl_ast_node_append_child($$, $predicate);
    }
    ;

join_using_constraint_name_list
    : unqualified_column_name[head] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_JOIN_USING_CONSTRAINT);
        $$ = dtl_ast_node_append_child($$, $head);
    }
    | join_using_constraint_name_list[prev] COMMA unqualified_column_name[next] {
        $$ = dtl_ast_node_append_child($prev, $next);
    }
    ;

join_using_constraint
    : USING OPEN_PARENTHESIS join_using_constraint_name_list[constraint] CLOSE_PARENTHESIS {
        $$ = $constraint;
    }
    ;

join_clause
    : JOIN table_binding join_constraint {
        $$ = dtl_ast_node_create_with_children(DTL_AST_JOIN_CLAUSE);
        $$ = dtl_ast_node_append_child($$, $table_binding);
        $$ = dtl_ast_node_append_child($$, $join_constraint);
    }
    ;

join_clause_list
    : join_clause_list[prev] join_clause[next] {
        if ($prev == NULL) {
            $$ = dtl_ast_node_create_with_children(DTL_AST_JOIN_CLAUSE_LIST);
        }
        $$ = dtl_ast_node_append_child($$, $next);
    }
    | %empty {
        $$ = NULL;
    }
    ;

where_clause
    : WHERE expression {
        $$ = dtl_ast_node_create_with_children(DTL_AST_WHERE_CLAUSE);
        $$ = dtl_ast_node_append_child($$, $expression);
    }
    | %empty {
        $$ = NULL;
    }
    ;

group_by_clause
    : GROUP BY expression_list[pattern] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_GROUP_BY_CLAUSE);
        $$ = dtl_ast_node_append_child($$, $pattern);
    }
    | GROUP CONSECUTIVE BY expression_list[pattern] {
        $$ = dtl_ast_node_create_with_children(DTL_AST_GROUP_CONSECUTIVE_BY_CLAUSE);
        $$ = dtl_ast_node_append_child($$, $pattern);
    }
    | %empty {
        $$ = NULL;
    }
    ;

table_expression
    : select_expression {
        $$ = $1;
    }
    | import_expression {
        $$ = $1;
    }
    | table_reference_expression {
        $$ = $1;
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
            $$ = dtl_ast_node_create_with_children(DTL_AST_SELECT_EXPRESSION);
            $$ = dtl_ast_node_append_child($$, $distinct);
            $$ = dtl_ast_node_append_child($$, $columns);
            $$ = dtl_ast_node_append_child($$, $source);
            $$ = dtl_ast_node_append_child($$, $joins);
            $$ = dtl_ast_node_append_child($$, $where);
            $$ = dtl_ast_node_append_child($$, $group_by);
        }
    ;

import_expression
    : IMPORT string {
        $$ = dtl_ast_node_create_with_children(DTL_AST_IMPORT_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $string);
    }
    ;

table_reference_expression
    : name {
        $$ = dtl_ast_node_create_with_children(DTL_AST_TABLE_REFERENCE_EXPRESSION);
        $$ = dtl_ast_node_append_child($$, $name);
    }
    ;

statement
    : assignment_statement {
        $$ = $1;
    }
//   | update_statement {
//        $$ = $1;
//    }
//   | delete_statement {
//        $$ = $1;
//    }
//    | insert_statement {
//        $$ = $1;
//    }
    | export_statement {
        $$ = $1;
    }
    ;

assignment_statement
    : WITH table_name AS table_expression SEMICOLON {
        $$ = dtl_ast_node_create_with_children(DTL_AST_ASSIGNMENT_STATEMENT);
        $$ = dtl_ast_node_append_child($$, $table_expression);
        $$ = dtl_ast_node_append_child($$, $table_name);  // Note that alias goes last to match convention everwhere else.
    }
    ;

export_statement
    : EXPORT table_expression TO string SEMICOLON {
        $$ = dtl_ast_node_create_with_children(DTL_AST_EXPORT_STATEMENT);
        $$ = dtl_ast_node_append_child($$, $table_expression);
        $$ = dtl_ast_node_append_child($$, $string);
    }
    ;

statement_list
    : %empty {
        $$ = NULL;
    }
    | statement_list[prev] statement[next] {
        if ($prev == NULL) {
            $$ = dtl_ast_node_create_with_children(DTL_AST_STATEMENT_LIST);
        }
        $$ = dtl_ast_node_append_child($$, $next);
    }
    ;

script
    : statement_list {
        $$ = dtl_ast_node_create_with_children(DTL_AST_SCRIPT);
        $$ = dtl_ast_node_append_child($$, $statement_list);
    }
    ;

result
    : script {
        *result = $1;
    }
    ;

%%
