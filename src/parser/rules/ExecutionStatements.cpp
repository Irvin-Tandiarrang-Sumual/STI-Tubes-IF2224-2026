#include "../Parser.hpp"

/**
 * Production Rule;
 * ASSIGNMENT-STATEMENT -> VARIABLE + becomes + EXPRESSION
 */
CSTNodes* Parser::parseAssignmentStatement(){
    CSTNodes* node = new CSTNodes(NonTerminal::ASSIGNMENT_STATEMENT, peek().codeLocation);
    node->addChild(parseVariable());
    node->addChild(new CSTNodes(expect(TokenType::becomes)));
    node->addChild(parseExpression());
    return node;
}

/**
 * Production Rule:
 * PROCEDURE-OR-FUNCTIONCALL -> ident + (lparent + PARAMETER-LIST? + rparent)
 */
CSTNodes* Parser::parseProcedureOrFunctionCall(){
    CSTNodes* node = new CSTNodes(NonTerminal::PROCEDURE_OR_FUNCTION_CALL, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    node->addChild(new CSTNodes(expect(TokenType::lparent)));
    if (peek().type == TokenType::plus
        || peek().type == TokenType::minus
        || peek().type == TokenType::ident
        || peek().type == TokenType::intcon
        || peek().type == TokenType::charcon
        || peek().type == TokenType::string
        || peek().type == TokenType::lparent
        || peek().type == TokenType::notsy
    ) node->addChild(parseParameterList());
    node->addChild(new CSTNodes(expect(TokenType::rparent)));
    return node;
}