#include "CSTNodes.hpp"

CSTNodes::CSTNodes(Token token, CodeLocation location) 
    : isTerminal_(true), value_(token), location_(location) {}

CSTNodes::CSTNodes(NonTerminal nonTerminal, CodeLocation location)
    : isTerminal_(false), value_(nonTerminal), location_(location) {}

void CSTNodes::addChild(std::unique_ptr<CSTNodes> newChild) {
    if (!isTerminal_) {
        // bisa tambahin child
        children_.push_back(std::move(newChild));
    }
}

bool CSTNodes::isTerminal() const {
    return isTerminal_;
}

NonTerminal CSTNodes::getNonTerminal() const {
    return std::get<NonTerminal>(value_);
}

const Token& CSTNodes::getToken() const {
    return std::get<Token>(value_);
}

const CodeLocation& CSTNodes::getLocation() const {
    return location_;
}
