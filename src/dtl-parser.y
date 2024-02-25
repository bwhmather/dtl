%define api.pure true
%define api.prefix {dtl_parser_}

%define parse.error verbose

%code top {
    #include "dtl-tokens.h"
}

%code requires {
    #include "dtl-ast.h"
    #include "dtl-tokenizer.h"
}

%union {
    struct dtl_token token;
    struct dtl_ast_node *node;
    struct dtl_ast_node **node_array;
}

%param {struct dtl_tokenizer *tokenizer}
%parse-param {struct dtl_ast_node **result}

%token <struct dtl_token> INT
%token <struct dtl_token> FLOAT
%token <struct dtl_token> STRING
%token <struct dtl_token> BYTE_STRING

/* Keywords. */
%token <struct dtl_token> UPDATE
%token <struct dtl_token> SELECT
%token <struct dtl_token> DISTINCT
%token <struct dtl_token> CONSECUTIVE
%token <struct dtl_token> AS
%token <struct dtl_token> FROM
%token <struct dtl_token> JOIN
%token <struct dtl_token> ON
%token <struct dtl_token> USING
%token <struct dtl_token> WHERE
%token <struct dtl_token> GROUP
%token <struct dtl_token> BY
%token <struct dtl_token> WITH
%token <struct dtl_token> IMPORT
%token <struct dtl_token> EXPORT
%token <struct dtl_token> TO

/* Symbols */
%token <struct dtl_token> SEMICOLON
%token <struct dtl_token> COMMA
%token <struct dtl_token> DOT
%token <struct dtl_token> OPEN_PAREN
%token <struct dtl_token> CLOSE_PAREN
%token <struct dtl_token> OPEN_BRACE
%token <struct dtl_token> CLOSE_BRACE
%token <struct dtl_token> OPEN_BRACKET
%token <struct dtl_token> CLOSE_BRACKET
%token <struct dtl_token> AT
%token <struct dtl_token> POUND
%token <struct dtl_token> TILDE
%token <struct dtl_token> QUESTION
%token <struct dtl_token> COLON
%token <struct dtl_token> DOLLAR

/* Operators */
%token <struct dtl_token> EQ
%token <struct dtl_token> NOT
%token <struct dtl_token> NOT_EQUAL
%token <struct dtl_token> LESS_THAN
%token <struct dtl_token> LESS_THAN_EQUAL
%token <struct dtl_token> GREATER_THAN
%token <struct dtl_token> GREATER_THAN_EQUAL
%token <struct dtl_token> MINUS
%token <struct dtl_token> PLUS
%token <struct dtl_token> STAR
%token <struct dtl_token> SLASH
%token <struct dtl_token> CARET
%token <struct dtl_token> PERCENT

/* Identifiers */
%token <struct dtl_token> TYPE
%token <struct dtl_token> NAME
%token <struct dtl_token> QUOTED_NAME

/* Blanks */
%token <struct dtl_token> LINE_COMMENT
%token <struct dtl_token> BLOCK_COMMENT
%token <struct dtl_token> WHITESPACE


%code {
    static int
    yylex(YYSTYPE *lvalp, struct dtl_tokenizer *tokenizer) {
        (void) lvalp;
        (void) tokenizer;

        struct dtl_token next = dtl_tokenizer_next(tokenizer);



        return 0;
    }

    static int
    yyerror(struct dtl_tokenizer *tokenizer, struct dtl_ast_node **result, const char *s) {
        (void) tokenizer;
        (void) result;
        (void) s;
        return 0;
    }
}


%left PLUS MINUS
%left STAR SLASH

%type <node> name;

%type <node> literal;
%type <node> string;

%type <node> column_name;
%type <node_array> unqualified_column_name_list;
%type <node> unqualified_column_name;
%type <node> qualified_column_name;

%type <node> expression;
%type <node> column_reference_expression;
%type <node> literal_expression;
%type <node_array> expression_list;
%type <node> function_call_expression;
%type <node> add_expression;
%type <node> subtract_expression;
%type <node> multiply_expression;
%type <node> divide_expression;

%type <node> table_name;
%type <node> distinct_clause;

%type <node> column_binding;
%type <node_array> column_binding_list;
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
%type <node_array> join_clause_list;

%type <node> where_clause;
%type <node> group_by_clause;

%type <node> table_expression;
%type <node> select_expression;
%type <node> import_expression;
%type <node> table_reference_expression;

%type <node> statement;
%type <node_array> statement_list;
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
        $$ = NULL;
    }
    | NAME {
        $$ = NULL;
    }
    ;

literal
    : string {
        $$ = $1;
    }
    ;

string
    : STRING {
        $$ = NULL;
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
    : name[column_name] {
        $$ = NULL;
    }
    ;

qualified_column_name
    : name[table_name] DOT name[column_name] {
        $$ = NULL;
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
        $$ = NULL;
    }
    ;

literal_expression
    : literal[value] {
        $$ = NULL;
    }
    ;

expression_list
    : expression[head] {
        $$ = NULL;
    }
    | expression_list[prev] COMMA expression[next] {
        $$ = NULL;
    }
    ;

function_call_expression
    : name OPEN_PAREN expression_list[arguments] CLOSE_PAREN {
        $$ = NULL;
    }
    ;

add_expression
    : expression[left] PLUS expression[right] {
        $$ = NULL;
    }
    ;

subtract_expression
    : expression[left] MINUS expression[right] {
        $$ = NULL;
    }
    ;

multiply_expression
    : expression[left] STAR expression[right] {
        $$ = NULL;
    }
    ;

divide_expression
    : expression[left] SLASH expression[right] {
        $$ = NULL;
    }
    ;

table_name
    : name {
        $$ = NULL;
    }
    ;

distinct_clause
    : DISTINCT {
        $$ = NULL;
    }
    | DISTINCT CONSECUTIVE {
        $$ = NULL;
    }
    | %empty {}
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
        $$ = NULL;
    }
    ;

implicit_column_binding
    : expression {
        $$ = NULL;
    }
    ;

aliased_column_binding
    : expression AS name[alias] {
        $$ = NULL;
    }
    ;

column_binding_list
    : column_binding[head] {
        $$ = NULL;
    }
    | column_binding_list[prev] COMMA column_binding[next] {
        $$ = NULL;
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
    : OPEN_PAREN table_expression CLOSE_PAREN {
        $$ = NULL;
    }
    | table_reference_expression {
        $$ = NULL;
    }
    ;

aliased_table_binding
    : OPEN_PAREN table_expression CLOSE_PAREN AS name {
        $$ = NULL;
    }
    | table_reference_expression AS name {
        $$ = NULL;
    }
    ;

from_clause
    : FROM table_binding {
        $$ = NULL;
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
        $$ = NULL;
    }
    ;

unqualified_column_name_list
    : unqualified_column_name[head] {
        $$ = NULL;
    }
    | unqualified_column_name_list[prev] COMMA unqualified_column_name[next] {
        $$ = NULL;
    }
    ;

join_using_constraint
    : USING OPEN_PAREN unqualified_column_name_list CLOSE_PAREN {
        $$ = NULL;
    }
    ;

join_clause
    : JOIN table_binding join_constraint {
        $$ = NULL;
    }
    ;

join_clause_list
    : %empty {}
    | join_clause_list[prev] join_clause[next] {
        $$ = NULL;
    }
    ;

where_clause
    : WHERE expression {
        $$ = NULL;
    }
    | %empty {}
    ;

group_by_clause
    : GROUP BY expression_list[pattern] {
        $$ = NULL;
    }
    | GROUP CONSECUTIVE BY expression_list[pattern] {
        $$ = NULL;
    }
    | %empty {}
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
            $$ = NULL;
        }
    ;

import_expression
    : IMPORT string {
        $$ = NULL;
    }
    ;

table_reference_expression
    : name {
        $$ = NULL;
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
        $$ = NULL;
    }
    ;

export_statement
    : EXPORT table_expression TO string SEMICOLON {
        $$ = NULL;
    }
    ;

statement_list
    : %empty {}
    | statement_list[prev] statement[next] {
        $$ = NULL;
    }
    ;

script
    : statement_list {
        $$ = NULL;
    }
    ;

result
    : script {
        *result = $1;
    }
    ;

%%
