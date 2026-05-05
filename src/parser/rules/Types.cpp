#include "../Parser.hpp"

/*
    TYPE : ident | ARRAY-TYPE | RANGE | ENUMERATED | RECORD-TYPE
*/
CSTNodes* Parser::parseType() {
    CSTNodes* node = new CSTNodes(NonTerminal::TYPE, peek().codeLocation);
    
    switch (peek().type) {
        case TokenType::arraysy:
            node->addChild(parseArrayType());
            break;
        case TokenType::lparent:
            node->addChild(parseEnumerated());
            break;
        case TokenType::recordsy:
            node->addChild(parseRecordType());
            break;
        case TokenType::ident:
            node->addChild(new CSTNodes(expect(TokenType::ident)));
            break;
        default:
            break;
    }
    
    return node;
}

/*
    ARRAY-TYPE : arraysy + lbrack + (RANGE | ident) + rbrack + ofsy + TYPE
*/
CSTNodes* Parser::parseArrayType() {
    CSTNodes* node = new CSTNodes(NonTerminal::ARRAY_TYPE, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::arraysy)));
    node->addChild(new CSTNodes(expect(TokenType::lbrack)));
    
    if (checkMultiple({TokenType::ident, TokenType::charcon, TokenType::intcon})) {
        if (peekNext().type == TokenType::period) {
            node->addChild(parseRange());
        } else {
            node->addChild(new CSTNodes(expect(TokenType::ident)));
        }
    }
    
    node->addChild(new CSTNodes(expect(TokenType::rbrack)));
    node->addChild(new CSTNodes(expect(TokenType::ofsy)));
    node->addChild(parseType());
    return node;
}

/*
    RANGE : CONSTANT + period + period + CONSTANT
*/
CSTNodes* Parser::parseRange() {
    CSTNodes* node = new CSTNodes(NonTerminal::RANGE, peek().codeLocation);
    node->addChild(parseConstant());
    node->addChild(new CSTNodes(expect(TokenType::period)));
    node->addChild(new CSTNodes(expect(TokenType::period)));
    node->addChild(parseConstant());
    return node;
}

/*
    ENUMERATED : lparent + ident + (comma + ident)* + rparent
*/
CSTNodes* Parser::parseEnumerated() {
    CSTNodes* node = new CSTNodes(NonTerminal::ENUMERATED, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::lparent)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    
    while (peek().type == TokenType::comma) {
        node->addChild(new CSTNodes(expect(TokenType::comma)));
        node->addChild(new CSTNodes(expect(TokenType::ident)));
    }
    
    node->addChild(new CSTNodes(expect(TokenType::rparent)));
    return node;
}

/*
    RECORD-TYPE : recordsy + FIELD-LIST + endsy
*/
CSTNodes* Parser::parseRecordType() {
    CSTNodes* node = new CSTNodes(NonTerminal::RECORD_TYPE, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::recordsy)));
    node->addChild(parseFieldList());
    node->addChild(new CSTNodes(expect(TokenType::endsy)));
    return node;
}

/*
    FIELD-LIST : FIELD-PART + (semicolon + FIELD-PART)*
*/
CSTNodes* Parser::parseFieldList() {
    CSTNodes* node = new CSTNodes(NonTerminal::FIELD_LIST, peek().codeLocation);
    node->addChild(parseFieldPart());
    
    while (check(TokenType::semicolon)) {
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));
        node->addChild(parseFieldPart());
    }
    
    return node;
}

/*
    FIELD-PART : IDENTIFIER-LIST + colon + TYPE
*/
CSTNodes* Parser::parseFieldPart() {
    CSTNodes* node = new CSTNodes(NonTerminal::FIELD_PART, peek().codeLocation);
    node->addChild(parseIdentifierList());
    node->addChild(new CSTNodes(expect(TokenType::colon)));
    node->addChild(parseType());
    return node;
}
