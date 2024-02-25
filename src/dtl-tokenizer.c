#include "dtl-tokenizer.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "dtl-location.h"
#include "dtl-tokens.h"

struct dtl_tokenizer {
    const char *input;
    const char *next;

    size_t lineno;
    size_t column;
};

static bool
dtl_tokenizer_is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

static bool
dtl_tokenizer_is_id_start(char c) {
    if ('a' <= c && c <= 'z') {
        return true;
    }

    if ('A' <= c && c <= 'Z') {
        return true;
    }

    if (c == '_') {
        return true;
    }

    return false;
}

static bool
dtl_tokenizer_is_id_continue(char c) {
    if ('a' <= c && c <= 'z') {
        return true;
    }

    if ('A' <= c && c <= 'Z') {
        return true;
    }

    if (c == '_') {
        return true;
    }

    if ('0' <= c && c <= '9') {
        return true;
    }

    return false;
}

static char
dtl_tokenizer_bump(struct dtl_tokenizer *tokenizer) {
    if (*tokenizer->next == '\0') {
        return '\0';
    }

    if (*tokenizer->next == '\n') {
        tokenizer->lineno += 1;
        tokenizer->column = 0;
    }

    tokenizer->column += 1;
    return *tokenizer->next++;
}

char
dtl_tokenizer_peek(struct dtl_tokenizer *tokenizer) {
    return *tokenizer->next;
}

enum dtl_token_type
dtl_tokenizer_next_type(struct dtl_tokenizer *tokenizer) {
    char curr = dtl_tokenizer_bump(tokenizer);

    if (dtl_tokenizer_is_whitespace(curr)) {
        while (dtl_tokenizer_is_whitespace(dtl_tokenizer_peek(tokenizer))) {
            dtl_tokenizer_bump(tokenizer);
        }
        return DTL_TOKEN_WHITESPACE;
    }

    if (curr == '\0') {
        return DTL_TOKEN_END_OF_FILE;
    }

    if (curr == '/') {
        if (dtl_tokenizer_peek(tokenizer) == '/') {
            dtl_tokenizer_bump(tokenizer);

            /* Consume everything up to the end of the line. */
            while (dtl_tokenizer_peek(tokenizer) != '\n' && dtl_tokenizer_peek(tokenizer) != '\0') {
                dtl_tokenizer_bump(tokenizer);
            }

            return DTL_TOKEN_LINE_COMMENT;

        } else if (dtl_tokenizer_peek(tokenizer) == '*') {
            while (true) {
                curr = dtl_tokenizer_bump(tokenizer);
                if (curr == '\0') {
                    return DTL_TOKEN_ERROR;
                }

                if (curr == '*' && dtl_tokenizer_peek(tokenizer) == '/') {
                    dtl_tokenizer_bump(tokenizer);
                    return DTL_TOKEN_BLOCK_COMMENT;
                }
            }

        } else {
            return DTL_TOKEN_SLASH;
        }
    }

    if (curr == '"') {
        while (true) {
            curr = dtl_tokenizer_bump(tokenizer);
            if (curr == '\\') {
                dtl_tokenizer_bump(tokenizer);
            }
            if (curr == '"') {
                return DTL_TOKEN_QUOTED_NAME;
            }
        }
    }

    if (curr == '\'') {
        while (true) {
            curr = dtl_tokenizer_bump(tokenizer);
            if (curr == '\0') {
                return DTL_TOKEN_ERROR;
            }
            if (curr == '\'') {
                if (dtl_tokenizer_peek(tokenizer) == '\'') {
                    dtl_tokenizer_bump(tokenizer);
                } else {
                    return DTL_TOKEN_STRING;
                }
            }
        }
    }

    if ('0' <= curr && curr <= '9') {
        /* TODO floats/octal/hex */
        while (
            dtl_tokenizer_peek(tokenizer) != '\0' &&
            '0' <= dtl_tokenizer_peek(tokenizer) &&
            dtl_tokenizer_peek(tokenizer) <= '9'
        ) {
            dtl_tokenizer_bump(tokenizer);
        }
        return DTL_TOKEN_INT;
    }

    if (dtl_tokenizer_is_id_start(curr)) {
        struct {
            const char *pattern;
            enum dtl_token_type type;
            bool matched;
        } patterns[] = {
            {"AS", DTL_TOKEN_AS, true},
            {"BY", DTL_TOKEN_BY, true},
            {"CONSECUTIVE", DTL_TOKEN_CONSECUTIVE, true},
            {"DISTINCT", DTL_TOKEN_DISTINCT, true},
            {"EXPORT", DTL_TOKEN_EXPORT, true},
            {"FROM", DTL_TOKEN_FROM, true},
            {"GROUP", DTL_TOKEN_GROUP, true},
            {"IMPORT", DTL_TOKEN_IMPORT, true},
            {"JOIN", DTL_TOKEN_JOIN, true},
            {"ON", DTL_TOKEN_ON, true},
            {"SELECT", DTL_TOKEN_SELECT, true},
            {"TO", DTL_TOKEN_TO, true},
            {"UPDATE", DTL_TOKEN_UPDATE, true},
            {"USING", DTL_TOKEN_USING, true},
            {"WHERE", DTL_TOKEN_WHERE, true},
            {"WITH", DTL_TOKEN_WITH, true},
        };

        bool capitalized = 'A' <= curr && curr <= 'Z';

        // Yes, I know this is slower than memcmp, but it's still nice to do things in one pass.
        size_t offset = 0;
        while (dtl_tokenizer_is_id_continue(curr)) {
            for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
                if (offset > strlen(patterns[i].pattern)) {
                    patterns[i].matched = false;
                    continue;
                }
                if (curr != patterns[i].pattern[offset]) {
                    patterns[i].matched = false;
                    continue;
                }
            }
            curr = dtl_tokenizer_bump(tokenizer);
            offset++;
        }
        for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
            if (!patterns[i].matched) {
                continue;
            }
            if (offset != strlen(patterns[i].pattern)) {
                continue;
            }
            return patterns[i].type;
        }

        if (capitalized) {
            return DTL_TOKEN_TYPE;
        }

        return DTL_TOKEN_NAME;
    }

    if (curr == ';') {
        return DTL_TOKEN_SEMICOLON;
    }

    if (curr == ',') {
        return DTL_TOKEN_COMMA;
    }

    if (curr == '.') {
        return DTL_TOKEN_DOT;
    }

    if (curr == '(') {
        return DTL_TOKEN_OPEN_PARENTHESIS;
    }

    if (curr == ')') {
        return DTL_TOKEN_CLOSE_PARENTHESIS;
    }

    if (curr == '{') {
        return DTL_TOKEN_OPEN_BRACE;
    }

    if (curr == '}') {
        return DTL_TOKEN_CLOSE_BRACE;
    }

    if (curr == '[') {
        return DTL_TOKEN_OPEN_BRACKET;
    }

    if (curr == ']') {
        return DTL_TOKEN_CLOSE_BRACKET;
    }

    if (curr == '@') {
        return DTL_TOKEN_AT;
    }

    if (curr == '#') {
        return DTL_TOKEN_POUND;
    }

    if (curr == '~') {
        return DTL_TOKEN_TILDE;
    }

    if (curr == '?') {
        return DTL_TOKEN_QUESTION;
    }

    if (curr == ':') {
        return DTL_TOKEN_COLON;
    }

    if (curr == '$') {
        return DTL_TOKEN_DOLLAR;
    }

    if (curr == '=') {
        return DTL_TOKEN_EQUAL;
    }

    if (curr == '!') {
        if (dtl_tokenizer_peek(tokenizer) == '=') {
            dtl_tokenizer_bump(tokenizer);
            return DTL_TOKEN_NOT_EQUAL;
        }
        return DTL_TOKEN_NOT;
    }

    if (curr == '<') {
        if (dtl_tokenizer_peek(tokenizer) == '=') {
            dtl_tokenizer_bump(tokenizer);
            return DTL_TOKEN_LESS_THAN_OR_EQUAL;
        }
        return DTL_TOKEN_LESS_THAN;
    }

    if (curr == '>') {
        if (dtl_tokenizer_peek(tokenizer) == '=') {
            dtl_tokenizer_bump(tokenizer);
            return DTL_TOKEN_GREATER_THAN_OR_EQUAL;
        }
        return DTL_TOKEN_GREATER_THAN;
    }

    if (curr == '-') {
        return DTL_TOKEN_MINUS;
    }

    if (curr == '+') {
        return DTL_TOKEN_PLUS;
    }

    if (curr == '*') {
        return DTL_TOKEN_STAR;
    }

    if (curr == '/') {
        return DTL_TOKEN_SLASH;
    }

    if (curr == '^') {
        return DTL_TOKEN_CARET;
    }

    if (curr == '%') {
        return DTL_TOKEN_PERCENT;
    }

    return DTL_TOKEN_ERROR;
}

struct dtl_token
dtl_tokenizer_next_token(struct dtl_tokenizer *tokenizer) {
    struct dtl_location start = {
        .offset = tokenizer->next - tokenizer->input,
        .lineno = tokenizer->lineno,
        .column = tokenizer->column,
    };

    enum dtl_token_type type = dtl_tokenizer_next_type(tokenizer);

    struct dtl_location end = {
        .offset = tokenizer->next - tokenizer->input,
        .lineno = tokenizer->lineno,
        .column = tokenizer->column,
    };

    struct dtl_token token = {
        .type = type,
        .start = start,
        .end = end
    };

    return token;
}
