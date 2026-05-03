#include "Compiler.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::cout << "=== ARION LEXER ===\n\n";

    std::string filename;
    std::cout << "Masukkan path/nama file input: ";
    std::getline(std::cin, filename);

    if (filename.empty()) {
        std::cerr << "ERROR: nama file tidak boleh kosong.\n";
        return 1;
    }

    std::vector<std::filesystem::path> candidates = {
        std::filesystem::path(filename),
        std::filesystem::path("../test/milestone-2") / filename,
        std::filesystem::path("../test/milestone-2/valid_test") / filename,
        std::filesystem::path("../test/milestone-2/invalid_test") / filename
    };

    std::filesystem::path inputPath;
    for (const auto &candidate : candidates) {
        if (std::filesystem::exists(candidate)) {
            inputPath = candidate;
            break;
        }
    }

    if (inputPath.empty()) {
        std::cerr << "ERROR: file tidak ditemukan.\n";
        return 1;
    }

    try {
        std::cout << "Membuka file: " << inputPath.string() << "\n";
        Compiler compiler(inputPath);
        compiler.lexer();
        std::cout << "Selesai.\n";
    } catch (const std::exception &e) {
        std::cerr << "RUNTIME ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
