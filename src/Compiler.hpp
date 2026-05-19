#pragma once
#include <iostream>
#include <filesystem>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "writer/Writer.hpp"
#include "ast/ASTBuilder.hpp"
#include "ast/ASTNodes.hpp"
class Compiler {
    public:
        explicit Compiler(const std::filesystem::path &path, const std::string &outputDir);
        ~Compiler();

        void lexer();
        void parser();
        void semantic();

    private:
        std::filesystem::path inputPath;
        std::string outputDir;
        std::vector<Token> tokens_;
        //
        CSTNodes* cstRoot_;
        std::vector<std::string> cstErrors_;
};
