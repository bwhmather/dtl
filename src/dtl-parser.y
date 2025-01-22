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
    #include <string.h>

    #include "dtl-ast.h"
    #include "dtl-error.h"
    #include "dtl-tokenizer.h"
}

%union {
    struct dtl_token token;
    struct dtl_ast_node *node;
}

%param {struct dtl_tokenizer *tokenizer}
%parse-param {struct dtl_ast_node **result}
%parse-param {struct dtl_error **error}

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
    yyerror(YYLTYPE *llocp, struct dtl_tokenizer *tokenizer, struct dtl_ast_node **result, struct dtl_error **error, const char *s) {
        (void) llocp;
        (void) tokenizer;
        (void) result;
        dtl_set_error(error, dtl_error_create("Parse error: %s", s));
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
%type <node> equal_to_expression;
%type <node> less_than_expression;
%type <node> less_than_or_equal_to_expression;
%type <node> greater_than_expression;
%type <node> greater_than_or_equal_to_expression;
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
        $$ = dtl_ast_name_node_create("not-implemented-in-parser");  // TODO
        dtl_ast_node_update_bounds($$, $1.start, $1.end);

    }
    | NAME {
        size_t start = $1.start.offset;
        size_t end = $1.end.offset;
        size_t length = end - start;
        char const *input = dtl_tokenizer_get_input(tokenizer);

        char *buffer = calloc(1, length + 1);
        memcpy(buffer, &input[start], length);

        $$ = dtl_ast_name_node_create(buffer);
        dtl_ast_node_update_bounds($$, $1.start, $1.end);

        free(buffer);
    }
    ;

literal
    : string {
        $$ = $1;
    }
    ;

string
    : STRING {
        size_t start = $1.start.offset;
        size_t end = $1.end.offset;
        char const *input = dtl_tokenizer_get_input(tokenizer);

        size_t length = 0;
        for (size_t i = start + 1; i < end - 1; i++) {
            // TODO unescape
            length += 1;
        }

        char *buffer = calloc(1, length + 1);
        size_t cursor = 0;
        for (size_t i = start + 1; i < end - 1; i++) {
            // TODO unescape.
            buffer[cursor] = input[i];
            cursor += 1;
        }

        $$ = dtl_ast_string_literal_node_create(buffer);
        dtl_ast_node_update_bounds($$, $1.start, $1.end);

        free(buffer);
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
        $$ = dtl_ast_unqualified_column_name_node_create($name);
    }
    ;

qualified_column_name
    : name[table_name] DOT name[column_name] {
        $$ = dtl_ast_qualified_column_name_node_create($table_name, $column_name);
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
    | equal_to_expression {
        $$ = $1;
    }
    | less_than_expression {
        $$ = $1;
    }
    | less_than_or_equal_to_expression {
        $$ = $1;
    }
    | greater_than_expression {
        $$ = $1;
    }
    | greater_than_or_equal_to_expression {
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
        $$ = dtl_ast_column_reference_expression_node_create($name);
    }
    ;

literal_expression
    : literal[value] {
        $$ = dtl_ast_literal_expression_node_create($value);
    }
    ;

expression_list
    : expression[head] {
        $$ = dtl_ast_expression_list_node_create($head);
    }
    | expression_list[prev] COMMA expression[next] {
        $$ = dtl_ast_expression_list_node_append($prev, $next);
    }
    ;

function_call_expression
    : name OPEN_PARENTHESIS expression_list[arguments] CLOSE_PARENTHESIS {
        $$ = dtl_ast_function_call_expression_node_create($name, $arguments);
    }
    ;
equal_to_expression
    : expression[left] EQUAL expression[right] {
        $$ = dtl_ast_equal_to_expression_node_create($left, $right);
    }
    ;
less_than_expression
    : expression[left] LESS_THAN expression[right] {
        $$ = dtl_ast_less_than_expression_node_create($left, $right);
    }
    ;
less_than_or_equal_to_expression
    : expression[left] LESS_THAN_OR_EQUAL expression[right] {
        $$ = dtl_ast_less_than_or_equal_to_expression_node_create($left, $right);
    }
    ;
greater_than_expression
    : expression[left] GREATER_THAN expression[right] {
        $$ = dtl_ast_greater_than_expression_node_create($left, $right);
    }
    ;
greater_than_or_equal_to_expression
    : expression[left] GREATER_THAN_OR_EQUAL expression[right] {
        $$ = dtl_ast_greater_than_or_equal_to_expression_node_create($left, $right);
    }
    ;
add_expression
    : expression[left] PLUS expression[right] {
        $$ = dtl_ast_add_expression_node_create($left, $right);
    }
    ;

subtract_expression
    : expression[left] MINUS expression[right] {
        $$ = dtl_ast_subtract_expression_node_create($left, $right);
    }
    ;

multiply_expression
    : expression[left] STAR expression[right] {
        $$ = dtl_ast_multiply_expression_node_create($left, $right);
    }
    ;

divide_expression
    : expression[left] SLASH expression[right] {
        $$ = dtl_ast_divide_expression_node_create($left, $right);
    }
    ;

table_name
    : name {
        $$ = dtl_ast_table_name_node_create($name);
    }
    ;

distinct_clause
    : DISTINCT {
        $$ = dtl_ast_distinct_clause_node_create();
        dtl_ast_node_update_bounds($$, $1.start, $1.end);
    }
    | DISTINCT CONSECUTIVE {
        $$ = dtl_ast_distinct_consecutive_clause_node_create();
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
        $$ = dtl_ast_wildcard_column_binding_node_create();
        dtl_ast_node_update_bounds($$, $1.start, $1.end);
    }
    ;

implicit_column_binding
    : expression {
        $$ = dtl_ast_implicit_column_binding_node_create($expression);
    }
    ;

aliased_column_binding
    : expression AS name[alias] {
        $$ = dtl_ast_aliased_column_binding_node_create($expression, $alias);
    }
    ;

column_binding_list
    : column_binding[head] {
        $$ = dtl_ast_column_binding_list_node_create();
        $$ = dtl_ast_column_binding_list_node_append($$, $head);
    }
    | column_binding_list[prev] COMMA column_binding[next] {
        $$ = dtl_ast_column_binding_list_node_append($prev, $next);
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
        $$ = dtl_ast_implicit_table_binding_node_create($expression);
    }
    | table_reference_expression[expression] {
        $$ = dtl_ast_implicit_table_binding_node_create($expression);
    }
    ;

aliased_table_binding
    : OPEN_PARENTHESIS table_expression[expression] CLOSE_PARENTHESIS AS name[alias] {
        $$ = dtl_ast_aliased_table_binding_node_create($expression, $alias);
    }
    | table_reference_expression[expression] AS name[alias] {
        $$ = dtl_ast_aliased_table_binding_node_create($expression, $alias);
    }
    ;

from_clause
    : FROM table_binding {
        $$ = dtl_ast_from_clause_node_create($table_binding);
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
        $$ = dtl_ast_join_on_constraint_node_create($predicate);
    }
    ;

join_using_constraint_name_list
    : unqualified_column_name[head] {
        $$ = dtl_ast_join_using_constraint_node_create($head);
    }
    | join_using_constraint_name_list[prev] COMMA unqualified_column_name[next] {
        $$ = dtl_ast_join_using_constraint_node_append($prev, $next);
    }
    ;

join_using_constraint
    : USING OPEN_PARENTHESIS join_using_constraint_name_list[constraint] CLOSE_PARENTHESIS {
        $$ = $constraint;
    }
    ;

join_clause
    : JOIN table_binding join_constraint {
        $$ = dtl_ast_join_clause_node_create($table_binding, $join_constraint);
    }
    | JOIN table_binding {
        $$ = dtl_ast_join_clause_node_create($table_binding, NULL);
    }
    ;

join_clause_list
    : join_clause_list[prev] join_clause[next] {
        if ($prev == NULL) {
            $$ = dtl_ast_join_clause_list_node_create($next);
        } else {
            $$ = dtl_ast_join_clause_list_node_append($$, $next);
        }
    }
    | %empty {
        $$ = NULL;
    }
    ;

where_clause
    : WHERE expression {
        $$ = dtl_ast_where_clause_node_create($expression);
    }
    | %empty {
        $$ = NULL;
    }
    ;

group_by_clause
    : GROUP BY expression_list[pattern] {
        $$ = dtl_ast_group_by_clause_node_create($pattern);
    }
    | GROUP CONSECUTIVE BY expression_list[pattern] {
        $$ = dtl_ast_group_consecutive_by_clause_node_create($pattern);
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
            $$ = dtl_ast_select_expression_node_create(
                $distinct,
                $columns,
                $source,
                $joins,
                $where,
                $group_by
            );
        }
    ;

import_expression
    : IMPORT string {
        $$ = dtl_ast_import_expression_node_create($string);
    }
    ;

table_reference_expression
    : name {
        $$ = dtl_ast_table_reference_expression_node_create($name);
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
        // Note that alias goes last to match convention everwhere else.
        $$ = dtl_ast_assignment_statement_node_create($table_expression, $table_name);
    }
    ;

export_statement
    : EXPORT table_expression TO string SEMICOLON {
        $$ = dtl_ast_export_statement_node_create($table_expression, $string);
    }
    ;

statement_list
    : %empty {
        $$ = NULL;
    }
    | statement_list[prev] statement[next] {
        if ($prev == NULL) {
            $$ = dtl_ast_statement_list_node_create();
        }
        $$ = dtl_ast_statement_list_node_append($$, $next);
    }
    ;

script
    : statement_list {
        $$ = dtl_ast_script_node_create($statement_list);
    }
    ;

result
    : script {
        *result = $1;
    }
    ;

%%
