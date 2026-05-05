#include "../Parser.hpp"

/*
    DECLARATION-PART : (CONST-DECLARATION)* + (TYPE-DECLARATION)* + (VAR-DECLARATION)* + (SUBPROGRAM-DECLARATION)*
*/

CSTNodes* Parser::parseDeclarationPart(){
    CSTNodes* node = new CSTNodes(NonTerminal::DECLARATION_PART, peek().codeLocation);
    while (peek().type == TokenType::constsy) {
        node->addChild(parseConstDeclaration());
    }
    while (peek().type == TokenType::typesy) {
        node->addChild(parseTypeDeclaration());
    }
    while (peek().type == TokenType::varsy) {
        node->addChild(parseVarDeclaration());
    }
    while (peek().type == TokenType::proceduresy ||
           peek().type == TokenType::functionsy) {
        node->addChild(parseSubprogramDeclaration());
    }
    return node;
}

/*
    CONST-DECLARATION : constsy + (ident + eql + CONSTANT + semicolon)+
*/
CSTNodes* Parser::parseConstDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::CONST_DECLARATION, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::constsy)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    node->addChild(new CSTNodes(expect(TokenType::eql)));
    node->addChild(parseConstant());
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    while (peek().type == TokenType::ident){
        node->addChild(new CSTNodes(expect(TokenType::ident)));
        node->addChild(new CSTNodes(expect(TokenType::eql)));
        node->addChild(parseConstant());
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));  
    }
    return node;
}

/*
    CONSTANT : charcon | string | [(plus | minus)? + (ident | intcon | realcon)]
*/
CSTNodes* Parser::parseConstant(){
    CSTNodes* node = new CSTNodes(NonTerminal::CONSTANT, peek().codeLocation);
    switch(peek().type){
        case TokenType::charcon:
            node->addChild(new CSTNodes(expect(TokenType::charcon))); 
            break;
        case TokenType::string:
            node->addChild(new CSTNodes(expect(TokenType::string)));  
            break;
        default:
            if (peek().type == TokenType::plus) node->addChild(new CSTNodes(expect(TokenType::plus))); 
            else if (peek().type == TokenType::minus) node->addChild(new CSTNodes(expect(TokenType::minus)));

            if (peek().type == TokenType::ident) node->addChild(new CSTNodes(expect(TokenType::ident))); 
            else if (peek().type == TokenType::intcon) node->addChild(new CSTNodes(expect(TokenType::intcon))); 
            else if (peek().type == TokenType::realcon) node->addChild(new CSTNodes(expect(TokenType::realcon))); 
    }
    return node;
}

/*
    TYPE-DECLARATION : typesy + (ident + eql + TYPE + semicolon)+
*/
CSTNodes* Parser::parseTypeDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::TYPE_DECLARATION, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::typesy)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    node->addChild(new CSTNodes(expect(TokenType::eql)));
    node->addChild(parseType());
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    while (peek().type == TokenType::ident){
        node->addChild(new CSTNodes(expect(TokenType::ident)));
        node->addChild(new CSTNodes(expect(TokenType::eql)));
        node->addChild(parseType());
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));  
    }
    return node;
}

/*
    VAR-DECLARATION : varsy + (IDENTIFIER-LIST + colon + TYPE + semicolon)+
*/
CSTNodes* Parser::parseVarDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::VAR_DECLARATION, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::varsy)));
    node->addChild(parseIdentifierList());
    node->addChild(new CSTNodes(expect(TokenType::colon)));
    node->addChild(parseType());
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    while (peek().type == TokenType::ident){
        node->addChild(parseIdentifierList());
        node->addChild(new CSTNodes(expect(TokenType::colon)));
        node->addChild(parseType());
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    }
    return node;
}

/*
    SUBPROGRAM-DECLARATION : PROCEDURE-DECLARATION | FUNCTION-DECLARATION
*/
CSTNodes* Parser::parseSubprogramDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::SUBPROGRAM_DECLARATION, peek().codeLocation);
    if (peek().type == TokenType::proceduresy) {
        node->addChild(parseProcedureDeclaration());
    } else if (peek().type == TokenType::functionsy) {
        node->addChild(parseFunctionDeclaration());
    }
    return node;
}


/*
    PROCEDURE-DECLARATION : proceduresy + ident + (FORMAL-PARAMETER-LIST)? + semicolon + BLOCK + semicolon
*/
CSTNodes* Parser::parseProcedureDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::PROCEDURE_DECLARATION, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::proceduresy)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    if (peek().type == TokenType::lparent) {
        node->addChild(parseFormalParameterList());
    }
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    node->addChild(parseBlock());
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    return node;
}

/*
    FUNCTION-DECLARATION : functionsy + ident + (FORMAL-PARAMETER-LIST)? + colon + ident + semicolon + BLOCK + semicolon
*/
CSTNodes* Parser::parseFunctionDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::FUNCTION_DECLARATION, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::functionsy)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    if (peek().type == TokenType::lparent) {
        node->addChild(parseFormalParameterList());
    }
    node->addChild(new CSTNodes(expect(TokenType::colon)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    node->addChild(parseBlock());
    node->addChild(new CSTNodes(expect(TokenType::semicolon)));
    return node;
}