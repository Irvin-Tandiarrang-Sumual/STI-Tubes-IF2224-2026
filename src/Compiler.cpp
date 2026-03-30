#include "Compiler.hpp"

Compiler::Compiler(const std::filesystem::path Path)
    : inputPath(Path) {}

Compiler::~Compiler() {}

// sekaligus ngelakuin write kali yak :/
void Compiler::lexer() {
    std::cout << "Processing Lexer\n";
    Lexer lexing(inputPath);

    std::cout << "I'm here after lexing (inputPath)\n";

    std::vector<Token> tokens = lexing.tokenize();

    std::cout << "Finish Tokenize\n";
    std::cout << "Going to write the result into result-tc-1.txt\n";
    Writer write("result-tc-1.txt", tokens);
    write.writeToFile();
}