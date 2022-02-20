#include "dtl-tokenizer.hpp"
#include "dtl-tokens.hpp"

#include "dtl-test.hpp"

int main(void) {
    std::string source = (
        "input = IMPORT 'input';\n"
        "output = SELECT old_column AS new_column FROM input;\n"
        "EXPORT output TO 'output';\n"
    );

    dtl::tokenizer::Tokenizer tokenizer(source);

    size_t count = 0;
    while (tokenizer.next_token().type != dtl::tokens::EndOfFile) {
        count++;
    }

    dtl_assert(count == 35);
}
