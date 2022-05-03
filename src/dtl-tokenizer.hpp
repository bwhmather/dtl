#pragma once

#include <iostream>
#include <string>

#include "dtl-tokens.hpp"

namespace dtl {
namespace tokenizer {

class Tokenizer {
    size_t m_lineno;
    size_t m_column;

    std::string::iterator m_next;
    std::string::iterator m_end;

    char bump();
    char peek();
    dtl::tokens::TokenType next_type();
public:
    Tokenizer(
        std::string::iterator begin,
        std::string::iterator end
    ) : m_lineno(1), m_column(1), m_next(begin), m_end(end) {}
    explicit Tokenizer(std::string& source) : Tokenizer(source.begin(), source.end()) {}
    Tokenizer(const Tokenizer&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;

    dtl::tokens::Token next_token();
};

}  /* namespace tokenizer */
}  /* namespace dtl */

