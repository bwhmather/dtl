#include "dtl-parser.hpp"
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
    auto script = dtl::parser::parse(tokenizer);
}
