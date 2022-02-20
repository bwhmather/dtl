#include "dtl-tokens.hpp"
#include "dtl-tokenizer.hpp"

#include "dtl-test.hpp"

int main(void) {
    std::string source = (
        "+\n"
        "-\n"
    );

    dtl::tokenizer::Tokenizer tokenizer(source);

    auto token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::Plus);
    dtl_assert(token.start.offset == source.begin());
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == source.begin() + 1);
    dtl_assert(token.end.lineno == 1);
    dtl_assert(token.end.column == 2);

    token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::Whitespace);
    dtl_assert(token.start.offset == source.begin() + 1);
    dtl_assert(token.start.lineno == 1);
    dtl_assert(token.start.column == 2);
    dtl_assert(token.end.offset == source.begin() + 2);
    dtl_assert(token.end.lineno == 2);
    dtl_assert(token.end.column == 1);

    token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::Minus);
    dtl_assert(token.start.offset == source.begin() + 2);
    dtl_assert(token.start.lineno == 2);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == source.begin() + 3);
    dtl_assert(token.end.lineno == 2);
    dtl_assert(token.end.column == 2);

    token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::Whitespace);
    dtl_assert(token.start.offset == source.begin() + 3);
    dtl_assert(token.start.lineno == 2);
    dtl_assert(token.start.column == 2);
    dtl_assert(token.end.offset == source.begin() + 4);
    dtl_assert(token.end.lineno == 3);
    dtl_assert(token.end.column == 1);

    token = tokenizer.next_token();
    dtl_assert(token.type == dtl::tokens::EndOfFile);
    dtl_assert(token.start.offset == source.begin() + 4);
    dtl_assert(token.start.lineno == 3);
    dtl_assert(token.start.column == 1);
    dtl_assert(token.end.offset == source.begin() + 4);
    dtl_assert(token.end.lineno == 3);
    dtl_assert(token.end.column == 1);
}
