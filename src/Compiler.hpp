#pragma once
#include <iostream>
#include <filesystem>
#include "../lexer/Lexer.hpp"

class Compiler
{
    public:
        explicit Compiler(const std::filesystem::path Path);
        ~Compiler();

        void lexer();

        // parser etc method

        std::filesystem::path Path;

};

