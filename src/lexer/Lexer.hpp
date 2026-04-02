#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "../reader/Reader.hpp"
#include "Token.hpp"

class Lexer {
private:
    std::filesystem::path path;
    std::vector<Token> tokens_;
    std::vector<std::string> errors_;
    Reader reader;

    void addToken(const Token &token);
    void addError(const CodeLocation &loc, const std::string &message, const std::string &lexeme = "");

    void skipWhitespace();
    void processToken();

    void processIdentifierOrKeyword();
    void processNumber();
    void processStringOrCharacter();
    void processCommentFromBrace(const CodeLocation &loc);
    void processCommentFromParen(const CodeLocation &loc);
    void processUnknownCharacter();
    void processMalformedIdentifier();
    void processMalformedRealStartingWithDot();
    void processSingleEqualsError();

    std::string readWhileIdentifierBody();
    std::string readUntilDelimiter();
    static bool isIdentifierBody(char ch);
    static bool isDelimiter(char ch);
    static std::string lowercase(const std::string &text);

public:
    explicit Lexer(const std::filesystem::path &p);
    ~Lexer();

    bool isEOF() const;
    CodeLocation getCodeLocation() const;
    void tokenize();

    const std::vector<std::string> &getErrors() const;
    const std::vector<Token> &getTokens() const;
};
