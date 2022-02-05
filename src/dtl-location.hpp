#pragma once

#include <cstddef>
#include <string>

namespace dtl {

struct Location {
    /* Byte offset of the beginning character in the string. */
    std::string::iterator offset;

    /* One-indexed line number of the character in the file */
    size_t lineno;

    /* One-indexed column offset of the character in the file */
    size_t column;
};

}  /* namespace dtl */
