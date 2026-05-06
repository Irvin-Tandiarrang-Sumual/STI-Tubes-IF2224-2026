#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens) {}

Parser::~Parser() = default;

CSTNodes* Parser::parse() {
    return parseProgram();
}

const Token& Parser::peek() const {
    return tokens_[currentPosition_];
}

const Token& Parser::peekNext() const {
    return tokens_[currentPosition_ + 1];
}
std::string Parser::makeErrorMessage(const std::string& expected) {
    return "Syntax error at line " +
        std::to_string(peek().codeLocation.line) +
        ", col " +
        std::to_string(peek().codeLocation.col) +
        ": expected '" + expected +
        "', got '" + tokenTypeToString(peek().type) + "'";
}

CSTNodes* Parser::expect(TokenType token) {
    if (check(token)) {
        CSTNodes* newNodes = new CSTNodes(peek());
        advance();
        return newNodes;
    }
    std::string expected = tokenTypeToString(token);
    std::string errorMessage = makeErrorMessage(expected);
    CSTNodes* errorNode = new CSTNodes(errorMessage, peek().codeLocation);
    addError(errorMessage); // nambahin

    synchronize({TokenType::semicolon, TokenType::endsy, 
            TokenType::elsesy, TokenType::untilsy});

    // sync dulu
    return errorNode;
}

void Parser::advance() {
    if (currentPosition_ < tokens_.size()) {
        currentPosition_++;
    }
    skipUselessToken();
}

void Parser::synchronize(std::vector<TokenType> syncSet) {
    for (auto t : syncSet) {
        if (check(t)) return;
    }
    if (!isAtEnd()) {
        advance();
    }

    while (!isAtEnd()) {
        if (check(TokenType::semicolon)) {
            advance();
            return;
        }

        for (auto tok : syncSet) {
            if (check(tok)) {
                return;
            }
        }

        advance();
    }
}

std::vector<std::string> Parser::getErrors() const {
    return errorMessages_;
}

void Parser::addError(const std::string& msg) {
    errorMessages_.push_back(msg);
}

CSTNodes* Parser::errorNode(std::string message) {
    return new CSTNodes(message, peek().codeLocation);
}

void Parser::skipUselessToken() {
    while (currentPosition_ < tokens_.size() &&
        (tokens_[currentPosition_].type == TokenType::comment)) {
        currentPosition_++;
    }
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return tokens_[currentPosition_].type == type;
}

bool Parser::checkMultiple(const std::vector<TokenType>& tokenTypes) const {
    if (isAtEnd()) return false;
    for (size_t i = 0; i < tokenTypes.size(); i++) {
        if (tokens_[currentPosition_].type == tokenTypes[i]) {
            return true;
        }
    }
    return false;
    
}
bool Parser::isAtEnd() const {
    return currentPosition_ >= tokens_.size();
}
