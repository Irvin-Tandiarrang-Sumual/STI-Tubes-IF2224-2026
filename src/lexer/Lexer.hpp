#pragma once
#include <filesystem>

#include "Token.hpp"
#include "../reader/Reader.hpp"
#include "../reader/CodeLocation.hpp"

#include <vector>

class Lexer
{
    private:
        /* data */
        std::filesystem::path path;
        std::vector<Token> tokens_;
        std::vector<std::string> errors_;
        Reader reader = Reader(path);
        Token currentToken = Token(TokenType::invalid_token);

        // private method
        void addErrors();
        void addTokens();

        void processToken();
        void processStringOrCharacter();
        void processKeywordOrIdentifier();
        void processNumber();

        void skippingWhiteSpaces();
        void skippingComments();


    public:
        explicit Lexer(const std::filesystem::path p);
        ~Lexer();

        bool isEOF() const;
        CodeLocation getCodeLocation() const;
        void advance(); // go to next token

        // do lexical analysis for 1 whole
        void tokenize();

        // getter
        std::vector<std::string> getErrors();
        std::vector<Token> getTokens();
        




        


};