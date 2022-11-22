#include "dtl-tokenizer.hpp"
#include "dtl-tokens.hpp"

#include "dtl-test.hpp"

int
main(void) {
    std::string source = "AS";
    dtl::tokenizer::Tokenizer tokenizer(source);

    auto token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::As);
    dtl_assert(token.start.offset == source.begin());
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == source.begin() + 2);
    dtl_assert(token.end.lineno == 1);
    dtl_assert(token.end.column == 3);

    token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::EndOfFile);
}
