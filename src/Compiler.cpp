#include "Compiler.hpp"

Compiler::Compiler(const std::filesystem::path Path)
    : inputPath(Path) {}

Compiler::~Compiler() {}

// sekaligus ngelakuin write kali yak :/
void Compiler::lexer() {
    std::cout << "Processing Lexer...\n";
    Lexer lexing(inputPath);

    std::cout << "I'm here after lexing (inputPath)\n";
    lexing.tokenize();

    std::vector<Token> tokens = lexing.getTokens();

    // tulis error
    std::cout << "\n";
    for (size_t i = 0; i < lexing.getErrors().size(); i++) {
        std::cout << lexing.getErrors().at(i);
    }
    std::cout << "\n";
    
    std::string baseName = inputPath.stem().string(); 
    std::string resultFileName = baseName + "-result.txt";
    std::filesystem::path outputDir = "../test/milestone-1/output";
    std::filesystem::path fullPath = outputDir / resultFileName;

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    std::cout << "Finish Tokenize for: " << inputPath.filename().string() << "\n";
    
    Writer write(fullPath.string(), tokens); 
    write.writeToFile();
    
    std::cout << "File saved at: " << fullPath.string() << "\n";
}