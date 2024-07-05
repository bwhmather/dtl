#pragma once

struct dtl_string_interner;

struct dtl_string_interner *
dtl_string_interner_create(void);

void
dtl_string_interner_destroy(struct dtl_string_interner *);

char const *
dtl_string_interner_intern(struct dtl_string_interner *, char const *);
