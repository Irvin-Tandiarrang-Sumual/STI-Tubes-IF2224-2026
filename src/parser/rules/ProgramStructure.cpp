#include "../Parser.hpp"

/*
    PROGRAM : PROGRAM-HEADER + DECLARATION-PART + COMPOUND-STATEMENT + period
*/
CSTNodes* Parser::parseProgram() {
    CSTNodes* node = new CSTNodes(NonTerminal::PROGRAM, peek().codeLocation);
    node->addChild(parseProgramHeader());
    node->addChild(parseDeclarationPart());
    node->addChild(parseCompoundStatement());
    node->addChild(expect(TokenType::period));

    return node;
}

/*
    PROGRAM-HEADER : programsy + ident + semicolon
*/
CSTNodes* Parser::parseProgramHeader() {
    CSTNodes* node = new CSTNodes(NonTerminal::PROGRAM_HEADER, peek().codeLocation);
    node->addChild(expect(TokenType::programsy));
    node->addChild(expect(TokenType::ident));
    node->addChild(expect(TokenType::semicolon));
    return node;
}

/*
    BLOCK : DECLARATION-PART + COMPOUND-STATEMENT
*/
CSTNodes* Parser::parseBlock() {
    CSTNodes* node = new CSTNodes(NonTerminal::BLOCK, peek().codeLocation);
    node->addChild(parseDeclarationPart());
    node->addChild(parseCompoundStatement());
    return node;
}