#include "dtl-parser.hpp"


static std::shared_ptr<arrow::Table> load_input_file(std::string& path) {
    std::shard_ptr<arrow::io::ReadableFile> input_file;
    PARQUET_ASSIGN_OR_THROW(
        input_file,
        arrow::io::ReadableFile::Open(input_path, arrow::default_memory_pool())
    );

    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_THROW_NOT_OK
        parquet::arrow::OpenFile(
            input_file, arrow::default_memory_pool(), &reader
        )
    );

    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
    return table;
}


static std::unordered_map<std::string, std::shared_ptr<arrow::Table>> load_input_dir(std::string) {

}


int main(int argc, char *argv[]) {
    assert(argc == 4);

    std::filesystem::path script_path(argv[1]);
    std::filesystem::path input_dir(argv[2]);
    std::filesystem::path output_dir(argv[3]);




    std::string source = (
        "input = IMPORT 'input';\n"
        "output = SELECT old_column AS new_column FROM input;\n"
        "EXPORT output TO 'output';\n"
    );

    dtl::tokenizer::Tokenizer tokenizer(source.begin(), source.end());
    dtl::parser::parse(tokenizer);
}
