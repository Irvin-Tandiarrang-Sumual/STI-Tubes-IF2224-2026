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

    const auto &tokens = lexing.getTokens();
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

    Writer writer(fullPath.string(), tokens);
    writer.writeToFile();
}
