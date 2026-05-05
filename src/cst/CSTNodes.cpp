#include "CSTNodes.hpp"

CSTNodes::CSTNodes(Token token)
    : isTerminal_(true), value_(token), location_(token.codeLocation) {}

CSTNodes::CSTNodes(NonTerminal nonTerminal, CodeLocation location)
    : isTerminal_(false), value_(nonTerminal),location_(location) {}

void CSTNodes::addChild(CSTNodes* newChild) {
    if (!isTerminal_) {
        // bisa tambahin child
        children_.push_back(newChild);
    }
}

bool CSTNodes::isTerminal() const {
    return isTerminal_;
}

const NonTerminal& CSTNodes::getNonTerminal() const {
    return std::get<NonTerminal>(value_);
}

const Token& CSTNodes::getToken() const {
    return std::get<Token>(value_);
}

const CodeLocation& CSTNodes::getLocation() const {
    return location_;
}

const std::vector<CSTNodes*>& CSTNodes::getChildren() const {
    return children_;
}

CSTNodes::~CSTNodes() {
    for (auto* child : children_) {
        delete child;
    }
}

const std::string CSTNodes::toString() const{
    if (isTerminal()) {
        const Token& token = getToken();
        std::string typeStr = tokenTypeToString(token.type);
        if (tokenNeedsValue(token.type)) {
            return typeStr + "(" + tokenValueToString(token) + ")";
        }
        return typeStr;
    }
    return "<" + nonTerminalToString(getNonTerminal()) + ">";
}