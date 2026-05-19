#include "ASTBuilder.hpp"

#include <sstream>

/*
    Entry point ASTBuilder.
*/
ASTProgramNode* ASTBuilder::build(const CSTNodes* root) {
    if (root == nullptr) {
        return nullptr;
    }
    if (root->isTerminal()) {
        return nullptr;
    }
    if (root->isError()) {
        return nullptr;
    }
    if (root->getNonTerminal() != NonTerminal::PROGRAM) {
        return nullptr;
    }
    return buildProgram(root);
}

/*
    Mengambil teks/value dari token terminal.
*/
std::string ASTBuilder::tokenText(const CSTNodes* node) const {
    if (node == nullptr) {
        return "";
    }
    if (node->isError()) {
        return "";
    }
    if (!node->isTerminal()) {
        return "";
    }
    return tokenValueToString(node->getToken());
}

/*
    Mengubah TokenType operator menjadi representasi string operator AST.
*/
std::string ASTBuilder::operatorText(TokenType type) const {
    switch (type) {
        case TokenType::plus:
            return "+";

        case TokenType::minus:
            return "-";

        case TokenType::times:
            return "*";

        case TokenType::rdiv:
            return "/";

        case TokenType::idiv:
            return "div";

        case TokenType::imod:
            return "mod";

        case TokenType::andsy:
            return "and";

        case TokenType::orsy:
            return "or";

        case TokenType::notsy:
            return "not";

        case TokenType::eql:
            return "==";

        case TokenType::neq:
            return "<>";

        case TokenType::gtr:
            return ">";

        case TokenType::geq:
            return ">=";

        case TokenType::lss:
            return "<";

        case TokenType::leq:
            return "<=";

        default:
            return "";
    }
}