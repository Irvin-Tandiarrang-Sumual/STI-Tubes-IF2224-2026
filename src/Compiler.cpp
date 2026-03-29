#include "Compiler.hpp"

Compiler::Compiler(const std::filesystem::path Path)
    : Path(Path) {}

Compiler::~Compiler() {}

void Compiler::lexer() {
    std::cout << "Processing Lexer\n";
}