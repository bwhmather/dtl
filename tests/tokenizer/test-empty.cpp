#include "dtl-tokenizer.hpp"
#include "dtl-tokens.hpp"

#include "dtl-test.hpp"

int
main(void) {
    std::string source = "";
    dtl::tokenizer::Tokenizer tokenizer(source);

    auto token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::EndOfFile);
    dtl_assert(token.start.offset == source.begin());
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == source.end());
    dtl_assert(token.end.lineno == 1);
    dtl_assert(token.end.column == 1);
}
