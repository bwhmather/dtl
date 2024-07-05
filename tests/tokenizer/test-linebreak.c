#include "dtl-tokenizer.h"
#include "dtl-tokens.h"

#include "dtl-test.h"

int
main(void) {
    char const *source = (
        "+\n"
        "-\n"
    );
    struct dtl_tokenizer *tokenizer;
    struct dtl_token token;

    tokenizer = dtl_tokenizer_create(source);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_PLUS);
    dtl_assert(token.start.offset == 0);
    dtl_assert(token.start.lineno == 0);
    dtl_assert(token.start.column == 0);
    dtl_assert(token.end.offset == 1);
    dtl_assert(token.end.lineno == 0);
    dtl_assert(token.end.column == 1);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_WHITESPACE);
    dtl_assert(token.start.offset == 1);
    dtl_assert(token.start.lineno == 0);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == 2);
    dtl_assert(token.end.lineno == 1);
    dtl_assert(token.end.column == 1);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_MINUS);
    dtl_assert(token.start.offset == 2);
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == 3);
    dtl_assert(token.end.lineno == 1);
    dtl_assert(token.end.column == 2);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_WHITESPACE);
    dtl_assert(token.start.offset == 3);
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 2);
    dtl_assert(token.end.offset == 4);
    dtl_assert(token.end.lineno == 2);
    dtl_assert(token.end.column == 1);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_END_OF_FILE);
    dtl_assert(token.start.offset == 4);
    dtl_assert(token.start.lineno == 2);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == 4);
    dtl_assert(token.end.lineno == 2);
    dtl_assert(token.end.column == 1);

    dtl_tokenizer_destroy(tokenizer);
}
