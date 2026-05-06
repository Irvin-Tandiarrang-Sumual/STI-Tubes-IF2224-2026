#include "../Parser.hpp"

/*
    DECLARATION-PART : (CONST-DECLARATION)* + (TYPE-DECLARATION)* + (VAR-DECLARATION)* + (SUBPROGRAM-DECLARATION)*
*/

CSTNodes* Parser::parseDeclarationPart(){
    CSTNodes* node = new CSTNodes(NonTerminal::DECLARATION_PART, peek().codeLocation);
    while (check(TokenType::constsy)) {
        node->addChild(parseConstDeclaration());
    }
    while (check(TokenType::typesy)) {
        node->addChild(parseTypeDeclaration());
    }
    while (check(TokenType::varsy)) {
        node->addChild(parseVarDeclaration());
    }
    while (checkMultiple({TokenType::proceduresy, TokenType::functionsy})) {
        node->addChild(parseSubprogramDeclaration());
    }
    return node;
}

/*
    CONST-DECLARATION : constsy + (ident + eql + CONSTANT + semicolon)+
*/
CSTNodes* Parser::parseConstDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::CONST_DECLARATION, peek().codeLocation);
    node->addChild(expect(TokenType::constsy));
    node->addChild(expect(TokenType::ident));
    node->addChild(expect(TokenType::eql));
    node->addChild(parseConstant());
    node->addChild(expect(TokenType::semicolon));
    while (check(TokenType::ident)){
        node->addChild(expect(TokenType::ident));
        node->addChild(expect(TokenType::eql));
        node->addChild(parseConstant());
        node->addChild(expect(TokenType::semicolon));  
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
            node->addChild(expect(TokenType::charcon));
            break;
        case TokenType::string:
            node->addChild(expect(TokenType::string));
            break;
        default:
            if (check(TokenType::plus)) node->addChild(expect(TokenType::plus)); 
            else if (check(TokenType::minus)) node->addChild(expect(TokenType::minus));

            if (check(TokenType::ident)) node->addChild(expect(TokenType::ident)); 
            else if (check(TokenType::intcon)) node->addChild(expect(TokenType::intcon)); 
            else if (check(TokenType::realcon)) node->addChild(expect(TokenType::realcon)); 
    }
    return node;
}

/*
    TYPE-DECLARATION : typesy + (ident + eql + TYPE + semicolon)+
*/
CSTNodes* Parser::parseTypeDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::TYPE_DECLARATION, peek().codeLocation);
    node->addChild(expect(TokenType::typesy));
    node->addChild(expect(TokenType::ident));
    node->addChild(expect(TokenType::eql));
    node->addChild(parseType());
    node->addChild(expect(TokenType::semicolon));
    while (check(TokenType::ident)){
        node->addChild(expect(TokenType::ident));
        node->addChild(expect(TokenType::eql));
        node->addChild(parseType());
        node->addChild(expect(TokenType::semicolon));  
    }
    return node;
}

/*
    VAR-DECLARATION : varsy + (IDENTIFIER-LIST + colon + TYPE + semicolon)+
*/
CSTNodes* Parser::parseVarDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::VAR_DECLARATION, peek().codeLocation);
    node->addChild(expect(TokenType::varsy));
    node->addChild(parseIdentifierList());
    node->addChild(expect(TokenType::colon));
    node->addChild(parseType());
    node->addChild(expect(TokenType::semicolon));
    while (check(TokenType::ident)){
        node->addChild(parseIdentifierList());
        node->addChild(expect(TokenType::colon));
        node->addChild(parseType());
        node->addChild(expect(TokenType::semicolon));
    }
    return node;
}

/*
    SUBPROGRAM-DECLARATION : PROCEDURE-DECLARATION | FUNCTION-DECLARATION
*/
CSTNodes* Parser::parseSubprogramDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::SUBPROGRAM_DECLARATION, peek().codeLocation);
    if (check(TokenType::proceduresy)) {
        node->addChild(parseProcedureDeclaration());
    } else if (check(TokenType::functionsy)) {
        node->addChild(parseFunctionDeclaration());
    }
    return node;
}


/*
    PROCEDURE-DECLARATION : proceduresy + ident + (FORMAL-PARAMETER-LIST)? + semicolon + BLOCK + semicolon
*/
CSTNodes* Parser::parseProcedureDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::PROCEDURE_DECLARATION, peek().codeLocation);
    node->addChild(expect(TokenType::proceduresy));
    node->addChild(expect(TokenType::ident));
    if (check(TokenType::lparent)) {
        node->addChild(parseFormalParameterList());
    }
    node->addChild(expect(TokenType::semicolon));
    node->addChild(parseBlock());
    node->addChild(expect(TokenType::semicolon));
    return node;
}

/*
    FUNCTION-DECLARATION : functionsy + ident + (FORMAL-PARAMETER-LIST)? + colon + ident + semicolon + BLOCK + semicolon
*/
CSTNodes* Parser::parseFunctionDeclaration(){
    CSTNodes* node = new CSTNodes(NonTerminal::FUNCTION_DECLARATION, peek().codeLocation);
    node->addChild(expect(TokenType::functionsy));
    node->addChild(expect(TokenType::ident));
    if (check(TokenType::lparent)) {
        node->addChild(parseFormalParameterList());
    }
    node->addChild(expect(TokenType::colon));
    node->addChild(expect(TokenType::ident));
    node->addChild(expect(TokenType::semicolon));
    node->addChild(parseBlock());
    node->addChild(expect(TokenType::semicolon));
    return node;
}