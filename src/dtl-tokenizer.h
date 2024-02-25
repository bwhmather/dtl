#pragma once

#include "dtl-tokens.h"

struct dtl_tokenizer;

struct dtl_tokenizer *
dtl_tokenizer_create(const char *input);

void
dtl_tokenizer_destroy(struct dtl_tokenizer *tokenizer);

struct dtl_token
dtl_tokenizer_next_token(struct dtl_tokenizer *tokenizer);
