#include "Engine.hpp"

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
        std::cerr << "  s  -> Menjalankan Lexer, Parser & Semantic\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string filename = argv[2];

    if (mode != "l" && mode != "p" && mode != "s") {
        std::cerr << "ERROR: Mode tidak valid. Gunakan 'l', 'p', atau 's'.\n";
        return 1;
    }

    std::string modeOut;
    if (mode == "l") {
        modeOut = "LEXER";
    } else if (mode == "p") {
        modeOut = "PARSER";
    } else if (mode == "s") {
        modeOut = "SEMANTIC";
    }

    std::cout << "=== ARION " << modeOut << " ===\n\n";

    std::string testFolder;
    if (mode == "l") {
        testFolder = "../test/milestone-1";
    } else if (mode == "p") {
        testFolder = "../test/milestone-2";
    } else {
        testFolder = "../test/milestone-3";
    }
    
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
        
        Engine engine(inputPath, outputDir);

        if (mode == "l") {
            engine.lexer();
        } else if (mode == "p") {
            engine.lexer();
            engine.parser();
        } else if (mode == "s") {
            engine.lexer();
            engine.parser();
            engine.semantic();
        }
        
        std::cout << "\nSelesai.\n";
    } catch (const std::exception &e) {
        std::cerr << "\nRUNTIME ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}