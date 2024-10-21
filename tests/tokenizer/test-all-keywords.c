#include "dtl-tokenizer.h"
#include "dtl-tokens.h"

#include "dtl-test.h"

int
main(void) {
    char const * source = (
        "AS\n"
        "BY\n"
        "CONSECUTIVE\n"
        "DISTINCT\n"
        "EXPORT\n"
        "FROM\n"
        "GROUP\n"
        "IMPORT\n"
        "JOIN\n"
        "ON\n"
        "SELECT\n"
        "TO\n"
        "UPDATE\n"
        "USING\n"
        "WHERE\n"
        "WITH"
    );
    char const *filename = "all-keywords.dtl";
    struct dtl_tokenizer *tokenizer;

    tokenizer = dtl_tokenizer_create(source, filename);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_AS);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_BY);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_CONSECUTIVE);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_DISTINCT);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_EXPORT);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_FROM);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_GROUP);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_IMPORT);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_JOIN);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_ON);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_SELECT);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_TO);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_UPDATE);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_USING);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHERE);
    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WHITESPACE);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_WITH);

    dtl_assert(dtl_tokenizer_next_token(tokenizer).type == DTL_TOKEN_END_OF_FILE);

    dtl_tokenizer_destroy(tokenizer);
}
