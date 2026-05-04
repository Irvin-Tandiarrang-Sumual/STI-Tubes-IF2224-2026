#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens) {}

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
        throw std::exception(); //
    }
    advance();
    return tokens_.at(currentPosition_);
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
