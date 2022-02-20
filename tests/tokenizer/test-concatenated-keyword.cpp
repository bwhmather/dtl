#include "dtl-tokens.hpp"
#include "dtl-tokenizer.hpp"

#include "dtl-test.hpp"

int main(void) {
    std::string source = "DISTINCTCONSECUTIVE";
    dtl::tokenizer::Tokenizer tokenizer(source);

    auto token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::Type);
    dtl_assert(token.start.offset == source.begin());
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == source.begin() + 19);
    dtl_assert(token.end.lineno == 1);
    dtl_assert(token.end.column == 20);

    token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::EndOfFile);
}
