#pragma once

#include <iostream>

#include "dtl-location.hpp"

namespace dtl {
namespace tokens {

enum TokenType {
    EndOfFile,
    Error,

    /* Literals. */
    Int,
    Float,
    String,
    ByteString,

    /* Keywords. */
    As,
    By,
    Consecutive,
    Distinct,
    Export,
    From,
    Group,
    Import,
    Join,
    On,
    Select,
    To,
    Update,
    Using,
    Where,
    With,

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

    /* Operators */
    Equal,
    Not,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Minus,
    Plus,
    Star,
    Slash,
    Caret,
    Percent,

    /* Identifiers */
    Type,
    Name,
    QuotedName,

    /* Blanks */
    LineComment,
    BlockComment,
    Whitespace
};

const char*
type_to_string(TokenType type);

std::ostream&
operator<<(std::ostream& os, const TokenType& type);

struct Token {
    TokenType type;
    Location start;
    Location end;
};

} /* namespace tokens */
} /* namespace dtl */
