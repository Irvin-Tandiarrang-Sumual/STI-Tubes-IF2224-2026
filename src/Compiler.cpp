#include "Compiler.hpp"

Compiler::Compiler(const std::filesystem::path Path)
    : inputPath(Path) {}

Compiler::~Compiler() {}

// sekaligus ngelakuin write kali yak :/
void Compiler::lexer() {
    std::cout << "Processing Lexer\n";
    Lexer lexing(inputPath);

    std::cout << "I'm here after lexing (inputPath)\n";
    lexing.tokenize();

    std::vector<Token> tokens = lexing.getTokens();

    // tulis error
    std::cout << "\n\n";
    for (size_t i = 0; i < lexing.getErrors().size(); i++) {
        std::cout << lexing.getErrors().at(i);
    }
    std::cout << "\n\n";
    

    std::cout << "Finish Tokenize\n";
    std::cout << "Going to write the result into result-tc-1.txt\n";
    Writer write("result-tc-1.txt", tokens);
    write.writeToFile();
}