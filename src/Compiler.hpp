#pragma once
#include <iostream>
#include <filesystem>
#include "lexer/Lexer.hpp"
#include "writer/Writer.hpp"
#include <vector>
class Compiler
{
    public:
        explicit Compiler(const std::filesystem::path path);
        ~Compiler();

        void lexer();

        // parser etc method

        std::filesystem::path inputPath;

};

