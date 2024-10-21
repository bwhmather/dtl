#include "dtl-tokenizer.h"
#include "dtl-tokens.h"

#include "dtl-test.h"

int
main(void) {
    char const *source = "DISTINCTCONSECUTIVE";
    char const *filename = "concatenated-keyword.dtl";
    struct dtl_tokenizer *tokenizer;
    struct dtl_token token;

    tokenizer = dtl_tokenizer_create(source, filename);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_TYPE);
    dtl_assert(token.start.offset == 0);
    dtl_assert(token.start.lineno == 0);
    dtl_assert(token.start.column == 0);
    dtl_assert(token.end.offset == 19);
    dtl_assert(token.end.lineno == 0);
    dtl_assert(token.end.column == 19);

    token = dtl_tokenizer_next_token(tokenizer);
    dtl_assert(token.type == DTL_TOKEN_END_OF_FILE);

    dtl_tokenizer_destroy(tokenizer);
}
