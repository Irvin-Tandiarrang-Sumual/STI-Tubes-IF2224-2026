#include "CRTNodes.hpp"

CRTNodes::CRTNodes(Token token) 
    : isTerminal_(true), value_(token) {}

CRTNodes::CRTNodes(Variable variable) 
    : isTerminal_(false), value_(variable) {}

void CRTNodes::addChild(std::unique_ptr<CRTNodes> newChild) {
    if (!isTerminal_) {
        // bisa tambahin child
        children_.push_back(std::move(newChild));
    }
}

bool CRTNodes::isTerminal() const {
    return isTerminal_;
}

Variable CRTNodes::getVariable() const {
    return std::get<Variable>(value_);
}

const Token& CRTNodes::getToken() const {
    return std::get<Token>(value_);
}
