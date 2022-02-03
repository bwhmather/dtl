#pragma once

#include <iostream>
#include <string>

namespace dtl {

enum class TokenType {
    Error,

    /* Literals. */
    Int,
    Float,
    Char,
    Byte,
    String,
    ByteString,
    RawString,
    RawByteString,

    /* Keywords. */
    Begin,
    Update,
    Select,
    Distinct,
    Consecutive,
    As,
    From,
    Join,
    On,
    Where,
    Group,
    By,
    With,
    Import,
    Export,
    To,
    /* Symbols */
    Semicolon,
    Comma,
    Dot,
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    OpenBracket,
    CloseBracket,
    At,
    Pound,
    Tilde,
    Question,
    Colon,
    Dollar,
    Eq,
    Not,
    NotEqual,
    LessThan,
    LessThanEqual,
    GreaterThan,
    GreaterThanEqual,
    Minus,
    MinusEqual,
    And,
    Or,
    Plus,
    PlusEqual,
    Star,
    StarEqual,
    Slash,
    SlashEqual,
    Caret,
    Percent,

    /* Identifiers */
    Type,
    Name,
    QuotedName,

    /* Blanks */
    LineComment,
    BlockComment,
    Whitespace,

};
const char * slow_token_type_to_string(TokenType type);
std::ostream& operator<<(std::ostream& os, const TokenType& type);


struct Location {
    /* Byte offset of the beginning character in the string. */
    offset;

    /* One-indexed line number of the character in the file */
    lineno;

    /* One-indexed column offset of the character in the file */
    column;
}

struct Token {
    TokenType type;

    /* Pointer to the first character in the token. */
    Location start;

    /* Pointer to the character after this token. */
    Location end;

    std::string text;
};


class Tokenizer {
    std::string::iterator m_next;
    std::string::iterator m_end;

    char bump();
    char peek();
    TokenType next_type();
public:
    Tokenizer(
        std::string::iterator begin,
        std::string::iterator end
    ) : m_next(begin), m_end(end) {}

    Token next_token();
    bool is_eof();
};

}

