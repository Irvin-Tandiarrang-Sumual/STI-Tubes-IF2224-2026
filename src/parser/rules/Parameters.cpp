#include "../Parser.hpp"

/*
    FORMAL-PARAMETER-LIST : lparent + PARAMETER-GROUP + (semicolon + PARAMETER-GROUP)* + rparent
*/
CSTNodes* Parser::parseFormalParameterList() {
    CSTNodes* node = new CSTNodes(NonTerminal::FORMAL_PARAMETER_LIST, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::lparent)));
    node->addChild(parseParameterGroup());
    
    while (peek().type == TokenType::semicolon) {
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));
        node->addChild(parseParameterGroup());
    }
    
    node->addChild(new CSTNodes(expect(TokenType::rparent)));
    return node;
}

/*
    PARAMETER-GROUP : IDENTIFIER-LIST + colon + (ident | ARRAY-TYPE)
*/
CSTNodes* Parser::parseParameterGroup() {
    CSTNodes* node = new CSTNodes(NonTerminal::PARAMETER_GROUP, peek().codeLocation);
    node->addChild(parseIdentifierList());
    node->addChild(new CSTNodes(expect(TokenType::colon)));
    
    if (peek().type == TokenType::arraysy) {
        node->addChild(parseArrayType());
    } else {
        node->addChild(new CSTNodes(expect(TokenType::ident)));
    }
    
    return node;
}

/*
    PARAMETER-LIST : EXPRESSION + (comma + EXPRESSION)*
*/
CSTNodes* Parser::parseParameterList() {
    CSTNodes* node = new CSTNodes(NonTerminal::PARAMETER_LIST, peek().codeLocation);
    node->addChild(parseExpression());
    
    while (peek().type == TokenType::comma) {
        node->addChild(new CSTNodes(expect(TokenType::comma)));
        node->addChild(parseExpression());
    }
    
    return node;
}
