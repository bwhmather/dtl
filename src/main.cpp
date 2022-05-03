#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "dtl-ast.hpp"
#include "dtl-ast-to-ir.hpp"
#include "dtl-eval.hpp"
#include "dtl-io-filesystem.hpp"
#include "dtl-ir.hpp"
#include "dtl-parser.hpp"
#include "dtl-tokenizer.hpp"

int main(int argc, char *argv[]) {
    assert(argc == 4);

    std::filesystem::path script_path(argv[1]);
    std::filesystem::path input_dir(argv[2]);
    std::filesystem::path output_dir(argv[3]);

    std::ifstream script_file(script_path);
    std::string source(std::istreambuf_iterator<char>(script_file), {});

    auto importer = dtl::io::filesystem_importer(input_dir);
    auto exporter = dtl::io::filesystem_exporter(output_dir);

    dtl::tokenizer::Tokenizer tokenizer(source.begin(), source.end());
    auto ast = dtl::parser::parse(tokenizer);
    auto program = dtl::ast::to_ir(*ast, *importer);

    dtl::eval::eval(program, *importer, *exporter);
}
