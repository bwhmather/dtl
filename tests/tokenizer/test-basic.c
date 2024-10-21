#include "dtl-tokenizer.h"
#include "dtl-tokens.h"

#include "dtl-test.h"

int
main(void) {
    char const *source = (
        "input = IMPORT 'input';\n"
        "output = SELECT old_column AS new_column FROM input;\n"
        "EXPORT output TO 'output';\n"
    );
    char const *filename = "basic.dtl";
    struct dtl_tokenizer *tokenizer;
    size_t count = 0;

    tokenizer = dtl_tokenizer_create(source, filename);

    while (dtl_tokenizer_next_token(tokenizer).type != DTL_TOKEN_END_OF_FILE) {
        count++;
    }

    dtl_assert(count == 35);

    dtl_tokenizer_destroy(tokenizer);
}
