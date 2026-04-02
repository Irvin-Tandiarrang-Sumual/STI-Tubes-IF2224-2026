#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../lexer/Token.hpp"


class Writer {
    private:
        const std::string filename;
        const std::vector<Token> tokens;

    public:
        Writer(const std::string& filename, const std::vector<Token>& tokens);
        ~Writer();

        void writeToFile() const;

};
