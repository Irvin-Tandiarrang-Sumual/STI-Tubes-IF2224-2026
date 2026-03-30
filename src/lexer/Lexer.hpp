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

        Reader reader = Reader(path);

        Token currentToken = Token(TokenType::invalid_token);

        Token processToken();
        Token processStringOrCharacter();
        Token processKeywordOrIdentifier();
        Token processNumber();

        void skippingWhiteSpaces();
        void skippingComments();
    public:
        explicit Lexer(const std::filesystem::path p);
        ~Lexer();

        bool isEOF() const;
        CodeLocation getCodeLocation() const;
        void advance(); // go to next token

        // do lexical analysis for 1 whole
        std::vector<Token> tokenize();
        




        


};