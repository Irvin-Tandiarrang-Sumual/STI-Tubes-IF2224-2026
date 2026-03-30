#pragma once
#include <ostream>
#include <string>
#include <variant>
#include <map>

#include "../reader/CodeLocation.hpp"

enum TokenType {
    eof, // additional ??
    invalid_token, // additional ??
    intcon,
    realcon,
    charcon,
    string,
    notsy, // 5
    plus,
    minus,
    times,
    idiv,
    rdiv, // 10
    imod,
    andsy,
    orsy,
    eql,
    neq, // 15
    gtr,
    geq,
    lss,
    leq,
    lparent, // 20
    rparent,
    lbrack,
    rbrack,
    comma,
    semicolon, // 25
    period,
    colon,
    becomes,
    constsy,
    typesy, // 30
    varsy,
    functionsy,
    proceduresy,
    arraysy,
    recordsy, // 35
    programsy,
    ident,
    beginsy,
    ifsy,
    casesy, // 40
    repeatsy,
    whilesy,
    forsy,
    endsy,
    elsesy, // 45
    untilsy,
    ofsy,
    dosy,
    tosy,
    downtosy, // 50
    thensy,
    comment,
};

struct Token {
    TokenType type;
    std::variant<int, double, std::string> value;
    CodeLocation codeLocation;

    explicit Token(TokenType type) : type(type) {}

    Token(TokenType type, const CodeLocation &loc) 
    : type(type), codeLocation(loc) {}
    
    // for integer value
    Token(TokenType type, int value, const CodeLocation &codeLocation)
        : type(type), value(value), codeLocation(codeLocation) {}

    // for double value
    Token(TokenType type, double value, const CodeLocation &codeLocation)
        : type(type), value(value), codeLocation(codeLocation) {}

    // for string value
    Token(TokenType type, const std::string &text, const CodeLocation &codeLocation)
        : type(type), value(text), codeLocation(codeLocation) {}
};

// return string based on token type
std::string tokenTypeToString(TokenType type);