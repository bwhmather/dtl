#pragma once

#include "dtl-tokens.h"

struct dtl_tokenizer;

struct dtl_tokenizer *
dtl_tokenizer_create(char const *input, char const *filename);

void
dtl_tokenizer_destroy(struct dtl_tokenizer *tokenizer);

struct dtl_token
dtl_tokenizer_next_token(struct dtl_tokenizer *tokenizer);

char const *
dtl_tokenizer_get_input(struct dtl_tokenizer *tokenizer);
