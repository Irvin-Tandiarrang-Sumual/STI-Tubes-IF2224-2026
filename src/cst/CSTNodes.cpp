#include "CSTNodes.hpp"

CSTNodes::CSTNodes(Token token) 
    : isTerminal_(true), value_(token) {}

CSTNodes::CSTNodes(Variable variable) 
    : isTerminal_(false), value_(variable) {}

void CSTNodes::addChild(std::unique_ptr<CSTNodes> newChild) {
    if (!isTerminal_) {
        // bisa tambahin child
        children_.push_back(std::move(newChild));
    }
}

bool CSTNodes::isTerminal() const {
    return isTerminal_;
}

Variable CSTNodes::getVariable() const {
    return std::get<Variable>(value_);
}

const Token& CSTNodes::getToken() const {
    return std::get<Token>(value_);
}
