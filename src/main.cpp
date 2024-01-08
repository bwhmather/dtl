#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "dtl-eval.hpp"
#include "dtl-io-filesystem.hpp"
#include "dtl-parser.hpp"

int
main(int argc, char* argv[]) {
    assert(argc == 5);

    std::filesystem::path script_path(argv[1]);
    std::filesystem::path input_dir(argv[2]);
    std::filesystem::path output_dir(argv[3]);
    std::filesystem::path trace_dir(argv[4]);

    std::ifstream script_file(script_path);
    std::string source(std::istreambuf_iterator<char>(script_file), {});

    auto importer = dtl::io::filesystem_importer(input_dir);
    auto exporter = dtl::io::filesystem_exporter(output_dir);
    auto tracer = dtl::io::filesystem_tracer(trace_dir);

    dtl::eval::run(source, *importer, *exporter, *tracer);
}
