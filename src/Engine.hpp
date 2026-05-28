#pragma once
#include <iostream>
#include <filesystem>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "writer/Writer.hpp"
#include "ast/ASTBuilder.hpp"
#include "ast/ASTNodes.hpp"
class Engine {
    public:
        explicit Engine(const std::filesystem::path &path, const std::string &outputDir);
        ~Engine();

        void lexer();
        void parser();
        void semantic();
        void intermediateCodeGenerator();

    private:
        std::filesystem::path inputPath;
        std::string outputDir;
        std::vector<Token> tokens_;
        //
        CSTNodes* cstRoot_;
        std::vector<std::string> cstErrors_;
};
