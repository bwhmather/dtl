#include "dtl-tokens.hpp"

#include <cassert>
#include <iostream>
#include <map>

namespace dtl {
namespace tokens {

const char*
type_to_string(TokenType type) {
    switch (type) {
    case TokenType::EndOfFile:
        return "<EndOfFile>";
    case TokenType::Error:
        return "<Error>";

    /* Literals. */
    case TokenType::Int:
        return "<Int>";
    case TokenType::Float:
        return "<Float>";
    case TokenType::String:
        return "<String>";
    case TokenType::ByteString:
        return "<ByteString>";

    /* Keywords. */
    case TokenType::As:
        return "<As>";
    case TokenType::By:
        return "<By>";
    case TokenType::Consecutive:
        return "<Consecutive>";
    case TokenType::Distinct:
        return "<Distinct>";
    case TokenType::Export:
        return "<Export>";
    case TokenType::From:
        return "<From>";
    case TokenType::Group:
        return "<Group>";
    case TokenType::Import:
        return "<Import>";
    case TokenType::Join:
        return "<Join>";
    case TokenType::On:
        return "<On>";
    case TokenType::Select:
        return "<Select>";
    case TokenType::To:
        return "<To>";
    case TokenType::Update:
        return "<Update>";
    case TokenType::Using:
        return "<Using>";
    case TokenType::Where:
        return "<Where>";
    case TokenType::With:
        return "<With>";

    /* Symbols */
    case TokenType::Semicolon:
        return "<Semicolon>";
    case TokenType::Comma:
        return "<Comma>";
    case TokenType::Dot:
        return "<Dot>";
    case TokenType::OpenParen:
        return "<OpenParen>";
    case TokenType::CloseParen:
        return "<CloseParen>";
    case TokenType::OpenBrace:
        return "<OpenBrace>";
    case TokenType::CloseBrace:
        return "<CloseBrace>";
    case TokenType::OpenBracket:
        return "<OpenBracket>";
    case TokenType::CloseBracket:
        return "<CloseBracket>";
    case TokenType::At:
        return "<At>";
    case TokenType::Pound:
        return "<Pound>";
    case TokenType::Tilde:
        return "<Tilde>";
    case TokenType::Question:
        return "<Question>";
    case TokenType::Colon:
        return "<Colon>";
    case TokenType::Dollar:
        return "<Dollar>";

    /* Operators */
    case TokenType::Eq:
        return "<Eq>";
    case TokenType::Not:
        return "<Not>";
    case TokenType::NotEqual:
        return "<NotEqual>";
    case TokenType::LessThan:
        return "<LessThan>";
    case TokenType::LessThanEqual:
        return "<LessThanEqual>";
    case TokenType::GreaterThan:
        return "<GreaterThan>";
    case TokenType::GreaterThanEqual:
        return "<GreaterThanEqual>";
    case TokenType::Minus:
        return "<Minus>";
    case TokenType::Plus:
        return "<Plus>";
    case TokenType::Star:
        return "<Star>";
    case TokenType::Slash:
        return "<Slash>";
    case TokenType::Caret:
        return "<Caret>";
    case TokenType::Percent:
        return "<Percent>";

    /* Identifiers */
    case TokenType::Type:
        return "<Type>";
    case TokenType::Name:
        return "<Name>";
    case TokenType::QuotedName:
        return "<QuotedName>";

    /* Blanks */
    case TokenType::LineComment:
        return "<LineComment>";
    case TokenType::BlockComment:
        return "<BlockComment>";
    case TokenType::Whitespace:
        return "<Whitespace>";

    default:
        assert(false); // ("unknown token type");
    }
}

std::ostream&
operator<<(std::ostream& os, const TokenType& type) {
    os << type_to_string(type);
    return os;
}

} /* namespace tokens */
} /* namespace dtl */
