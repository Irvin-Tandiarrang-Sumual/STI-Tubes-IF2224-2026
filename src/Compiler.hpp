#pragma once

#include <filesystem>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "writer/Writer.hpp"

class Compiler {
public:
    explicit Compiler(const std::filesystem::path &path, const std::string &outputDir);
    ~Compiler();

    void lexer();
    void parser();

private:
    std::filesystem::path inputPath;
    std::string outputDir;
    std::vector<Token> tokens_;
};
