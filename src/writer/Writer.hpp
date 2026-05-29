#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "../lexer/Token.hpp"
#include "../cst/CSTNodes.hpp"
#include "../ast/ASTNodes.hpp"
#include "../intermediate/Instruction.hpp"


class Writer {
    private:
        const std::string filename;
        const std::vector<Token> tokens;
        CSTNodes* root = nullptr;
        const std::vector<Instruction> instructions;
        const std::vector<std::string> errorMessages_;

        // print CST tree
        void writeCSTRecursive(std::ostream& out, const CSTNodes* node, const std::string& prefix, bool isLast, std::size_t depth) const;

        // print AST tree
        void writeASTRecursive(std::ostream& out, const ASTNode* node, const std::string& prefix, bool isLast, std::size_t depth) const;
        void writeDecoratedASTRecursive(std::ostream& out, const ASTNode* node, const std::string& prefix, bool isLast, std::size_t depth) const;
        std::string formatDecoratedNode(const ASTNode* node) const;
    public:
        Writer(const std::string& filename, const std::vector<Token>& tokens);
        
        // buat parser
        Writer(const std::string& filename, CSTNodes* root, const std::vector<std::string>& parserErrorMessages);

        // buat intermediate code
        Writer(const std::string& filename, const std::vector<Instruction>& instructions);
        ~Writer();

        void writeTokenToFile() const;


        void writeCSTToFile() const;
        void printCST() const;
        void printParserError() const;

        // AST output helpers (accept an AST root pointer)
        void writeASTToFile(const ASTNode* rootAst) const;
        void printAST(const ASTNode* rootAst) const;
        void writeDecoratedASTToFile(const ASTNode* rootAst) const;
        void printDecoratedAST(const ASTNode* rootAst) const;
        void writeDecoratedASTWithTablesToFile(const ASTNode* rootAst, const std::string& tablesText) const;
        void printDecoratedASTWithTables(const ASTNode* rootAst, const std::string& tablesText) const;

        // intermediate
        void writeIntermediateCodeToFile() const;
        void printIntermediateCode() const;

};
