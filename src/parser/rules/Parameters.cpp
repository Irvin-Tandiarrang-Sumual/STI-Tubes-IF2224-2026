#include "../Parser.hpp"

/*
    FORMAL-PARAMETER-LIST : lparent + PARAMETER-GROUP + (semicolon + PARAMETER-GROUP)* + rparent
*/
CSTNodes* Parser::parseFormalParameterList() {
    CSTNodes* node = new CSTNodes(NonTerminal::FORMAL_PARAMETER_LIST, peek().codeLocation);
    node->addChild(expect(TokenType::lparent));
    node->addChild(parseParameterGroup());
    
    while (check(TokenType::semicolon)) {
        node->addChild(expect(TokenType::semicolon));
        node->addChild(parseParameterGroup());
    }
    
    node->addChild(expect(TokenType::rparent));
    return node;
}

/*
    PARAMETER-GROUP : IDENTIFIER-LIST + colon + (ident | ARRAY-TYPE)
*/
CSTNodes* Parser::parseParameterGroup() {
    CSTNodes* node = new CSTNodes(NonTerminal::PARAMETER_GROUP, peek().codeLocation);
    node->addChild(parseIdentifierList());
    node->addChild(expect(TokenType::colon));
    
    if (check(TokenType::arraysy)) {
        node->addChild(parseArrayType());
    } else {
        node->addChild(expect(TokenType::ident));
    }
    
    return node;
}

/*
    PARAMETER-LIST : EXPRESSION + (comma + EXPRESSION)*
*/
CSTNodes* Parser::parseParameterList() {
    CSTNodes* node = new CSTNodes(NonTerminal::PARAMETER_LIST, peek().codeLocation);
    node->addChild(parseExpression());
    
    while (check(TokenType::comma)) {
        node->addChild(expect(TokenType::comma));
        node->addChild(parseExpression());
    }
    
    return node;
}
