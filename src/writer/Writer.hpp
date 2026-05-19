#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "../lexer/Token.hpp"
#include "../cst/CSTNodes.hpp"


class Writer {
    private:
        const std::string filename;
        const std::vector<Token> tokens;
        CSTNodes* root = nullptr;
        const std::vector<std::string> errorMessages_;

        // print tree
        void writeCSTRecursive(std::ostream& out, const CSTNodes* node, const std::string& prefix, bool isLast, std::size_t depth) const;
    public:
        Writer(const std::string& filename, const std::vector<Token>& tokens);
        
        // buat parser
        Writer(const std::string& filename, CSTNodes* root, const std::vector<std::string>& parserErrorMessages);
        ~Writer();

        void writeToFile() const;

        void writeCSTToFile() const;
        void printCST() const;
        void printParserError() const;

};
