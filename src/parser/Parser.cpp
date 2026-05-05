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

const Token Parser::expect(TokenType token) {
    if (peek().type != token) {
        std::string expected = tokenTypeToString(token);
        std::string got = tokenTypeToString(peek().type);
        throw std::runtime_error( "Syntax error at line " + std::to_string(peek().codeLocation.line) + ", col " + std::to_string(peek().codeLocation.col) + ": expected '" + expected + "', got '" + got + "'");
    }
    const Token consumed = tokens_.at(currentPosition_);
    advance();
    return consumed;
}

void Parser::advance() {
    if (currentPosition_ < tokens_.size()) {
        currentPosition_++;
    }
    skipUselessToken();

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

bool Parser::isAtEnd() const {
    return currentPosition_ >= tokens_.size();
}
