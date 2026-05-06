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

        // print tree
        void writeTreeRecursive(std::ostream& out, const CSTNodes* node, const std::string& prefix, bool isLast, std::size_t depth) const;
    public:
        Writer(const std::string& filename, const std::vector<Token>& tokens);
        
        // buat parser
        Writer(const std::string& filename, CSTNodes* root);
        ~Writer();

        void writeToFile() const;

        void writeTreeToFile() const;
        void printTree() const;

};
