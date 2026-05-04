#pragma once
#include "../Parser.hpp"

/**
 * Production Rule:
 * Compound-statement -> beginsy + Statement-list + endsy
 */
CSTNodes* Parser::parseCompoundStatement(){
    CSTNodes* node = new CSTNodes(NonTerminal::COMPOUND_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::beginsy)));
    node->addChild(parseStatementList());
    node->addChild(new CSTNodes(expect(TokenType::endsy)));
    return node;
}

/**
 * Production Rule:
 * Statement-list -> Statement + (semicolon + Statement)*
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

/**
 * Production Rule:
 * Statement -> (Assignment-statement | If-statement | Case-statement | While-statement | Repeat-statement | For-statement | Procedure/Function-call)?
 */

CSTNodes* Parser::parseStatement(){
    CSTNodes* node = new CSTNodes(NonTerminal::STATEMENT, peek().codeLocation);
    switch (peek().type) {
        case TokenType::ident:
            if (peekNext().type == TokenType::becomes ||
                peekNext().type == TokenType::period ||
                peekNext().type == TokenType::lbrack) {
                node->addChild(parseAssignmentStatement());
            } else {
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
