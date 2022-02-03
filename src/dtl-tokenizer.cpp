#include "slow-tokenizer.hpp"

#include <map>
#include <iostream>
#include <cassert>

#include "slow-error.hpp"


const char * slow_token_type_to_string(TokenType type) {
    switch(type) {
    case TokenType::Error:
        return "<Error>";
    case TokenType::Abstract:
        return "<Abstract>";
    case TokenType::As:
        return "<As>";
    case TokenType::Async:
        return "<Async>";
    case TokenType::Await:
        return "<Await>";
    case TokenType::Break:
        return "<Break>";
    case TokenType::Case:
        return "<Case>";
    case TokenType::Char:
        return "<Char>";
    case TokenType::Class:
        return "<Class>";
    case TokenType::Const:
        return "<Const>";
    case TokenType::Continue:
        return "<Continue>";
    case TokenType::Default:
        return "<Default>";
    case TokenType::Do:
        return "<Do>";
    case TokenType::Else:
        return "<Else>";
    case TokenType::Enum:
        return "<Enum>";
    case TokenType::Extern:
        return "<Extern>";
    case TokenType::Final:
        return "<Final>";
    case TokenType::Fn:
        return "<Fn>";
    case TokenType::For:
        return "<For>";
    case TokenType::Goto:
        return "<Goto>";
    case TokenType::If:
        return "<If>";
    case TokenType::Impl:
        return "<Impl>";
    case TokenType::In:
        return "<In>";
    case TokenType::False:
        return "<False>";
    case TokenType::True:
        return "<True>";
    case TokenType::Let:
        return "<Let>";
    case TokenType::Loop:
        return "<Loop>";
    case TokenType::Macro:
        return "<Macro>";
    case TokenType::Match:
        return "<Match>";
    case TokenType::Mod:
        return "<Mod>";
    case TokenType::OpenBrace:
        return "<OpenBrace>";
    case TokenType::CloseBrace:
        return "<CloseBrace>";
    case TokenType::OpenBracket:
        return "<OpenBracket>";
    case TokenType::CloseBracket:
        return "<CloseBracket>";
    case TokenType::OpenParen:
        return "<OpenParen>";
    case TokenType::CloseParen:
        return "<CloseParen>";
    case TokenType::Byte:
        return "<Byte>";
    case TokenType::ByteString:
        return "<ByteString>";
    case TokenType::RawByteString:
        return "<RawByteString>";
    case TokenType::RawIdent:
        return "<RawIdent>";
    case TokenType::RawString:
        return "<RawString>";
    case TokenType::Int:
        return "<Int>";
    case TokenType::Float:
        return "<Float>";
    case TokenType::And:
        return "<And>";
    case TokenType::At:
        return "<At>";
    case TokenType::Caret:
        return "<Caret>";
    case TokenType::Colon:
        return "<Colon>";
    case TokenType::Comma:
        return "<Comma>";
    case TokenType::Dollar:
        return "<Dollar>";
    case TokenType::Dot:
        return "<Dot>";
    case TokenType::Eq:
        return "<Eq>";
    case TokenType::GreaterThan:
        return "<GreaterThan>";
    case TokenType::GreaterThanEqual:
        return "<GreaterThanEqual>";
    case TokenType::Ident:
        return "<Ident>";
    case TokenType::LessThan:
        return "<LessThan>";
    case TokenType::LessThanEqual:
        return "<LessThanEqual>";
    case TokenType::Minus:
        return "<Minus>";
    case TokenType::MinusEqual:
        return "<MinusEqual>";
    case TokenType::Semicolon:
        return "<Semicolon>";
    case TokenType::Slash:
        return "<Slash>";
    case TokenType::SlashEqual:
        return "<SlashEqual>";
    case TokenType::Star:
        return "<Star>";
    case TokenType::StarEqual:
        return "<StarEqual>";
    case TokenType::String:
        return "<String>";
    case TokenType::Tilde:
        return "<Tilde>";
    case TokenType::Not:
        return "<Not>";
    case TokenType::NotEqual:
        return "<NotEqual>";
    case TokenType::Or:
        return "<Or>";
    case TokenType::Percent:
        return "<Percent>";
    case TokenType::Plus:
        return "<Plus>";
    case TokenType::PlusEqual:
        return "<PlusEqual>";
    case TokenType::Pound:
        return "<Pound>";
    case TokenType::Question:
        return "<Question>";
    case TokenType::LineComment:
        return "<LineComment>";
    case TokenType::BlockComment:
        return "<BlockComment>";
    case TokenType::Whitespace:
        return "<Whitespace>";
    default:
        slow_abort("unknown token type");
    }
}
std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    os << slow_token_type_to_string(type);
    return os;
}


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
        return '\0';
    }
    return *m_next++;
}

char Tokenizer::peek() {
    if (m_next == m_end) {
        return '\0';
    }
    return *m_next;
}

static const std::map<std::string, TokenType> keyword_map {
    { "abstract", TokenType::Abstract },
    { "as", TokenType::As },
    { "async", TokenType::Async },
    { "await", TokenType::Await },
    { "break", TokenType::Break },
    { "case", TokenType::Case },
    { "char", TokenType::Char },
    { "class", TokenType::Class },
    { "const", TokenType::Const },
    { "continue", TokenType::Continue },
    { "default", TokenType::Default },
    { "do", TokenType::Do },
    { "else", TokenType::Else },
    { "enum", TokenType::Enum },
    { "extern", TokenType::Extern },
    { "final", TokenType::Final },
    { "fn", TokenType::Fn },
    { "for", TokenType::For },
    { "goto", TokenType::Goto },
    { "if", TokenType::If },
    { "impl", TokenType::Impl },
    { "in", TokenType::In },
    { "false", TokenType::False },
    { "true", TokenType::True },
    { "let", TokenType::Let },
    { "loop", TokenType::Loop },
    { "macro", TokenType::Macro },
    { "match", TokenType::Match },
    { "mod", TokenType::Mod },
};

TokenType Tokenizer::next_type() {
    char curr = bump();

    if (is_whitespace(curr)) {
        while (is_whitespace(peek())) {
            bump();
        }
        return TokenType::Whitespace;
    }

    if (curr == '/') {
        if (peek() == '/') {
            bump();

            /* Consume everything up to the end of the line. */
            while (peek() != '\n' && peek() != '\0') {
                bump();
            }

            return TokenType::LineComment;

        } else if (peek() == '*') {
            while (true) {
                curr = bump();
                if (curr == '\0') {
                    return TokenType::Error;
                }

                if (curr == '*' && peek() == '/') {
                    bump();
                    return TokenType::BlockComment;
                }
            }

        } else {
            return TokenType::Slash;
        }
    }

    if (curr == '"') {
        while (true) {
            curr = bump();
            if (curr == '\\') {
                bump();
            }
            if (curr == '"') {
                return TokenType::String;
            }
        }
    }

    if (curr == '\'') {
        curr = bump();
        if (curr == '\\') {
            curr = bump();
        }
        curr = bump();
        if (curr != '\'') {
            return TokenType::Error;
        }
        return TokenType::Char;
    }

    if ('0' <= curr && curr <= '9') {
        /* TODO floats/octal/hex */
        while (peek() != '\0' && '0' <= peek() && peek() <= '9') {
            bump();
        }
        return TokenType::Int;
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
        return TokenType::Ident;
    }

    if (curr == ';') {
        return TokenType::Semicolon;
    }

    if (curr == ',') {
        return TokenType::Comma;
    }

    if (curr == '.') {
        return TokenType::Dot;
    }

    if (curr == '(') {
        return TokenType::OpenParen;
    }

    if (curr == ')') {
        return TokenType::CloseParen;
    }

    if (curr == '{') {
        return TokenType::OpenBrace;
    }

    if (curr == '}') {
        return TokenType::CloseBrace;
    }

    if (curr == '[') {
        return TokenType::OpenBracket;
    }

    if (curr == ']') {
        return TokenType::CloseBracket;
    }

    if (curr == '@') {
        return TokenType::At;
    }

    if (curr == '#') {
        return TokenType::Pound;
    }

    if (curr == '~') {
        return TokenType::Tilde;
    }

    if (curr == '?') {
        return TokenType::Question;
    }

    if (curr == ':') {
        return TokenType::Colon;
    }

    if (curr == '$') {
        return TokenType::Dollar;
    }

    if (curr == '=') {
        return TokenType::Eq;
    }

    if (curr == '!') {
        if (peek() == '=') {
            bump();
            return TokenType::NotEqual;
        }
        return TokenType::Not;
    }

    if (curr == '<') {
        if (peek() == '=') {
            bump();
            return TokenType::LessThanEqual;
        }
        return TokenType::LessThan;
    }

    if (curr == '>') {
        if (peek() == '=') {
            bump();
            return TokenType::GreaterThanEqual;
        }
        return TokenType::GreaterThan;
    }

    if (curr == '-') {
        if (peek() == '=') {
            bump();
            return TokenType::MinusEqual;
        }
        return TokenType::Minus;
    }

    if (curr == '&') {
        return TokenType::And;
    }

    if (curr == '|') {
        return TokenType::Or;
    }

    if (curr == '+') {
        if (peek() == '+') {
            bump();
            return TokenType::PlusEqual;
        }
        return TokenType::Plus;
    }

    if (curr == '*') {
        if (peek() == '=') {
            bump();
            return TokenType::StarEqual;
        }
        return TokenType::Star;
    }

    if (curr == '/') {
        if (peek() == '=') {
            bump();
            return TokenType::SlashEqual;
        }
        return TokenType::Slash;
    }

    if (curr == '^') {
        return TokenType::Caret;
    }

    if (curr == '%') {
        return TokenType::Percent;
    }

    return TokenType::Error;
}

Token Tokenizer::next_token() {
    auto start = m_next;
    TokenType type = next_type();
    auto end = m_next;

    return (Token){ .type=type, .start=start, .end=end };
}

bool Tokenizer::is_eof() {
    return m_next == m_end;
}


