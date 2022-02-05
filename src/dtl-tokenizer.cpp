#include "dtl-tokenizer.hpp"

#include <map>
#include <iostream>
#include <cassert>

#include "dtl-location.hpp"
#include "dtl-tokens.hpp"

namespace dtl {

static bool is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

static bool is_id_start(char c) {
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

static bool is_id_continue(char c) {
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

char Tokenizer::bump() {
    if (m_next == m_end) {
        m_lineno += 1;
        m_column = 1;
        return '\0';
    }
    m_column += 1;
    return *m_next++;
}

char dtl::Tokenizer::peek() {
    if (m_next == m_end) {
        return '\0';
    }
    return *m_next;
}

static const std::map<std::string, dtl::tokens::TokenType> keyword_map {
    { "BEGIN", dtl::tokens::Begin },
    { "UPDATE", dtl::tokens::Update },
    { "SELECT", dtl::tokens::Select },
    { "DISTINCT", dtl::tokens::Distinct },
    { "CONSECUTIVE", dtl::tokens::Consecutive },
    { "AS", dtl::tokens::As },
    { "FROM", dtl::tokens::From },
    { "JOIN", dtl::tokens::Join },
    { "ON", dtl::tokens::On },
    { "WHERE", dtl::tokens::Where },
    { "WITH", dtl::tokens::With },
    { "IMPORT", dtl::tokens::Import },
    { "EXPORT", dtl::tokens::Export },
};

dtl::tokens::TokenType Tokenizer::next_type() {
    char curr = bump();

    if (is_whitespace(curr)) {
        while (is_whitespace(peek())) {
            bump();
        }
        return dtl::tokens::Whitespace;
    }

    if (curr == '/') {
        if (peek() == '/') {
            bump();

            /* Consume everything up to the end of the line. */
            while (peek() != '\n' && peek() != '\0') {
                bump();
            }

            return dtl::tokens::LineComment;

        } else if (peek() == '*') {
            while (true) {
                curr = bump();
                if (curr == '\0') {
                    return dtl::tokens::Error;
                }

                if (curr == '*' && peek() == '/') {
                    bump();
                    return dtl::tokens::BlockComment;
                }
            }

        } else {
            return dtl::tokens::Slash;
        }
    }

    if (curr == '"') {
        while (true) {
            curr = bump();
            if (curr == '\\') {
                bump();
            }
            if (curr == '"') {
                return dtl::tokens::QuotedName;
            }
        }
    }

    if (curr == '\'') {
        while (true) {
            curr = bump();
            if (curr == '\\') {
                bump();
            }
            if (curr == '"') {
                return dtl::tokens::String;
            }
        }
    }

    if ('0' <= curr && curr <= '9') {
        /* TODO floats/octal/hex */
        while (peek() != '\0' && '0' <= peek() && peek() <= '9') {
            bump();
        }
        return dtl::tokens::Int;
    }

    if (is_id_start(curr)) {
        // TODO this could be done in a single pass.
        std::string token;

        token += curr;
        while (is_id_continue(peek())) {
            token += bump();
        }

        auto result = keyword_map.find(token);
        if (result != keyword_map.end()) {
            return result->second;
        }

        if ('A' <= token[0] && token[0] <= 'Z') {
            return dtl::tokens::Type;
        }

        return dtl::tokens::Name;
    }

    if (curr == ';') {
        return dtl::tokens::Semicolon;
    }

    if (curr == ',') {
        return dtl::tokens::Comma;
    }

    if (curr == '.') {
        return dtl::tokens::Dot;
    }

    if (curr == '(') {
        return dtl::tokens::OpenParen;
    }

    if (curr == ')') {
        return dtl::tokens::CloseParen;
    }

    if (curr == '{') {
        return dtl::tokens::OpenBrace;
    }

    if (curr == '}') {
        return dtl::tokens::CloseBrace;
    }

    if (curr == '[') {
        return dtl::tokens::OpenBracket;
    }

    if (curr == ']') {
        return dtl::tokens::CloseBracket;
    }

    if (curr == '@') {
        return dtl::tokens::At;
    }

    if (curr == '#') {
        return dtl::tokens::Pound;
    }

    if (curr == '~') {
        return dtl::tokens::Tilde;
    }

    if (curr == '?') {
        return dtl::tokens::Question;
    }

    if (curr == ':') {
        return dtl::tokens::Colon;
    }

    if (curr == '$') {
        return dtl::tokens::Dollar;
    }

    if (curr == '=') {
        return dtl::tokens::Eq;
    }

    if (curr == '!') {
        if (peek() == '=') {
            bump();
            return dtl::tokens::NotEqual;
        }
        return dtl::tokens::Not;
    }

    if (curr == '<') {
        if (peek() == '=') {
            bump();
            return dtl::tokens::LessThanEqual;
        }
        return dtl::tokens::LessThan;
    }

    if (curr == '>') {
        if (peek() == '=') {
            bump();
            return dtl::tokens::GreaterThanEqual;
        }
        return dtl::tokens::GreaterThan;
    }

    if (curr == '-') {
        if (peek() == '=') {
            bump();
            return dtl::tokens::MinusEqual;
        }
        return dtl::tokens::Minus;
    }

    if (curr == '&') {
        return dtl::tokens::And;
    }

    if (curr == '|') {
        return dtl::tokens::Or;
    }

    if (curr == '+') {
        if (peek() == '+') {
            bump();
            return dtl::tokens::PlusEqual;
        }
        return dtl::tokens::Plus;
    }

    if (curr == '*') {
        if (peek() == '=') {
            bump();
            return dtl::tokens::StarEqual;
        }
        return dtl::tokens::Star;
    }

    if (curr == '/') {
        if (peek() == '=') {
            bump();
            return dtl::tokens::SlashEqual;
        }
        return dtl::tokens::Slash;
    }

    if (curr == '^') {
        return dtl::tokens::Caret;
    }

    if (curr == '%') {
        return dtl::tokens::Percent;
    }

    return dtl::tokens::Error;
}

dtl::tokens::Token Tokenizer::next_token() {
    dtl::Location start = {
        .offset=m_next, .lineno=m_lineno, .column=m_column
    };

    dtl::tokens::TokenType type = next_type();

    dtl::Location end = {
        .offset=m_next, .lineno=m_lineno, .column=m_column
    };

    dtl::tokens::Token token = { .type=type, .start=start, .end=end };

    return token;
}

bool Tokenizer::is_eof() {
    return m_next == m_end;
}

}  /* namespace dtl */
