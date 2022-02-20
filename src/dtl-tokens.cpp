#include "dtl-tokens.hpp"

#include <map>
#include <iostream>
#include <cassert>

namespace dtl {
namespace tokens {

const char * dtl_token_type_to_string(TokenType type) {
    switch(type) {
    case TokenType::Error:
        return "<Error>";

    default:
        assert(false);  // ("unknown token type");
    }
}

std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    os << dtl_token_type_to_string(type);
    return os;
}

}  /* namespace tokens */
}  /* namespace dtl */
