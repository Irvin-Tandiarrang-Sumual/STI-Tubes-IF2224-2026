#include "../Parser.hpp"

/*
    COMPOUND-STATEMENT : beginsy + STATEMENT-LIST + endsy
*/
CSTNodes* Parser::parseCompoundStatement(){
    CSTNodes* node = new CSTNodes(NonTerminal::COMPOUND_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::beginsy)));
    node->addChild(parseStatementList());
    node->addChild(new CSTNodes(expect(TokenType::endsy)));
    return node;
}

/*
    STATEMENT-LIST : STATEMENT + (semicolon + STATEMENT)*
*/

CSTNodes* Parser::parseStatementList(){
    CSTNodes* node = new CSTNodes(NonTerminal::STATEMENT_LIST, peek().codeLocation);
    node->addChild(parseStatement());

    while (peek().type == TokenType::semicolon) {
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));
        node->addChild(parseStatement());
    }

    return node;
}

/*
    STATEMENT : ASSIGNMENT-STATEMENT | IF-STATEMENT | CASE-STATEMENT | WHILE-STATEMENT | REPEAT-STATEMENT | FOR-STATEMENT | PROCEDURE-OR-FUNCTION-CALL
*/

CSTNodes* Parser::parseStatement(){
    CSTNodes* node = new CSTNodes(NonTerminal::STATEMENT, peek().codeLocation);
    switch (peek().type) {
        case TokenType::ident:
            if (peekNext().type == TokenType::becomes ||
                peekNext().type == TokenType::period ||
                peekNext().type == TokenType::lbrack) {
                node->addChild(parseAssignmentStatement());
            } else if (peekNext().type == TokenType::lparent) {
                node->addChild(parseProcedureOrFunctionCall());
            }
            break;
        case TokenType::ifsy:
            node->addChild(parseIfStatement());
            break;
        case TokenType::casesy:
            node->addChild(parseCaseStatement());
            break;
        case TokenType::whilesy:
            node->addChild(parseWhileStatement());
            break;
        case TokenType::repeatsy:
            node->addChild(parseRepeatStatement());
            break;
        case TokenType::forsy:
            node->addChild(parseForStatement());
            break;
        default:
            break;
    }

    return node;
}
