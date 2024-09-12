#include "dtl-ast-find-imports.h"
#include "dtl-parser.h"
#include "dtl-tokenizer.h"
#include "dtl-tokens.h"

#include "dtl-test.h"

static void
count_imports(struct dtl_ast_node *node, void *user_data) {
    int *count = (int *) user_data;

    (void) node;

    *count += 1;
}

int
main(void) {
    char const *source = (
        "WITH input1 AS IMPORT 'input1';\n"
        "WITH input2 AS IMPORT 'input2';\n"
        "WITH output AS SELECT old_column AS new_column FROM input2;\n"
        "EXPORT output TO 'output';\n"
    );
    struct dtl_tokenizer *tokenizer;
    int parse_result;
    struct dtl_ast_node *root;
    int count = 0;

    tokenizer = dtl_tokenizer_create(source);

    parse_result = dtl_parser_parse(tokenizer, &root);
    dtl_assert(parse_result == 0);

    dtl_ast_find_imports(root, count_imports, &count);
    dtl_assert(count == 2);

    dtl_ast_node_destroy(root);

    dtl_tokenizer_destroy(tokenizer);
}
