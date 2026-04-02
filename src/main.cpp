#include "Compiler.hpp"
#include <iostream>
#include <string>
#include <filesystem>

int main () {
    std::cout << "=== ARION TOKENIZER (for now) ===\n\n";

    std::string filename;
    std::cout << "Masukkan nama file input (contoh: valid1.txt): ";
    std::cin >> filename;

    std::filesystem::path inputDir = "../test/milestone-1/valid_test";
    std::filesystem::path inputPath = inputDir / filename;

    if (!std::filesystem::exists(inputPath)) {
        std::cerr << "ERROR: File '" << filename << " not found in " << inputDir << "\n";
        return 1;
    }

    try {
        std::cout << "\nOpening: " << inputPath.string() << "...\n";
        
        Compiler compiler(inputPath);
        std::cout << "Compiler initialization success.\n";
        
        compiler.lexer();
        
        std::cout << "Done! Output file in ../test/milestone-1/output\n";
    } catch (const std::exception& e) {
        std::cerr << "RUNTIME ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}