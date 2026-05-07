#include "Compiler.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Cara penggunaan: " << argv[0] << " <mode> <nama_file>\n";
        std::cerr << "Mode:\n";
        std::cerr << "  l  -> Menjalankan Lexer\n";
        std::cerr << "  p  -> Menjalankan Lexer & Parser\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string filename = argv[2];

    if (mode != "l" && mode != "p") {
        std::cerr << "ERROR: Mode tidak valid. Gunakan 'l' atau 'p'.\n";
        return 1;
    }

    std::cout << "=== ARION " << (mode == "l" ? "LEXER" : "PARSER") << " ===\n\n";

    std::string testFolder = (mode == "l") ? "../test/milestone-1" : "../test/milestone-2";
    std::string outputDir = testFolder + "/output";

    std::vector<std::filesystem::path> candidates = {
        std::filesystem::path(filename),
        std::filesystem::path(testFolder) / filename,
        std::filesystem::path(testFolder) / "valid_test" / filename,
        std::filesystem::path(testFolder) / "unique_test" / filename
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
        
        Compiler compiler(inputPath, outputDir);

        if (mode == "l") {
            compiler.lexer();
        } else if (mode == "p") {
            compiler.lexer();
            compiler.parser();
        }
        
        std::cout << "\nSelesai.\n";
    } catch (const std::exception &e) {
        std::cerr << "\nRUNTIME ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
