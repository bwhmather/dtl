#include "dtl-tokenizer.hpp"
#include "dtl-tokens.hpp"

#include "dtl-test.hpp"

int
main(void) {
    std::string source =
        ("AS\n"
         "BY\n"
         "CONSECUTIVE\n"
         "DISTINCT\n"
         "EXPORT\n"
         "FROM\n"
         "GROUP\n"
         "IMPORT\n"
         "JOIN\n"
         "ON\n"
         "SELECT\n"
         "TO\n"
         "UPDATE\n"
         "USING\n"
         "WHERE\n"
         "WITH");

    dtl::tokenizer::Tokenizer tokenizer(source);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::As);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::By);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Consecutive);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Distinct);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Export);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::From);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Group);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Import);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Join);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::On);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Select);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::To);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Update);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Using);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::Where);
    dtl_assert(tokenizer.next_token().type == dtl::tokens::Whitespace);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::With);

    dtl_assert(tokenizer.next_token().type == dtl::tokens::EndOfFile);
}
