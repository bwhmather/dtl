#pragma once

#include <iostream>
#include <string>

#include "dtl-location.hpp"

namespace dtl {
namespace tokens {

enum TokenType {
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

struct Token {
    TokenType type;
    Location start;
    Location end;
};

}  /* namespace tokens */
}  /* namespace dtl */
