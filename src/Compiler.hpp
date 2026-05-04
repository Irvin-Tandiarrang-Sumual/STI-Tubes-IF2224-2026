#pragma once

#include <filesystem>

#include "lexer/Lexer.hpp"
#include "writer/Writer.hpp"

class Compiler {
public:
    explicit Compiler(const std::filesystem::path &path, const std::string &outputDir);
    ~Compiler();

    void lexer();

private:
    std::filesystem::path inputPath;
    std::string outputDir;
};
