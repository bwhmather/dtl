#pragma once

#include "dtl-ast.h"
#include "dtl-tokenizer.h"

int
dtl_parser_parse(struct dtl_tokenizer *tokenizer, struct dtl_ast_node **result);
