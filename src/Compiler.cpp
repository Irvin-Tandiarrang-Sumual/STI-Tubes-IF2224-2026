#include "Compiler.hpp"

#include <iostream>

Compiler::Compiler(const std::filesystem::path &path, const std::string &outputDir) 
    : inputPath(path), outputDir(outputDir) {}
Compiler::~Compiler() = default;

// sekaligus ngelakuin write kali yak :/
void Compiler::lexer() {
    std::cout << "Processing lexer...\n";
    Lexer lexing(inputPath);
    lexing.tokenize();

    tokens_ = lexing.getTokens();
    const auto &errors = lexing.getErrors();

    if (!errors.empty()) {
        std::cout << "\n=== Lexical Errors ===\n";
        for (const auto &error : errors) {
            std::cout << error;
        }
        std::cout << "======================\n\n";
    }

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-result.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), tokens_);
    writer.writeTokenToFile();
}

void Compiler::parser() {
    if (tokens_.empty()) {
        throw std::runtime_error("Syntax error: no tokens");
    }

    std::cout << "Processing parser...\n";
    Parser parser(tokens_);
    CSTNodes* root = parser.parse();

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-parse-tree.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), root, parser.getErrors());
    writer.printCST();
    writer.writeCSTToFile();
    writer.printParserError();

    delete root;
}
