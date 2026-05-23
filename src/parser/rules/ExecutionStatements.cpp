#include "../Parser.hpp"

/**
 * Production Rule;
 * ASSIGNMENT-STATEMENT -> VARIABLE + becomes + EXPRESSION
 */
CSTNodes* Parser::parseAssignmentStatement(){
    CSTNodes* node = new CSTNodes(NonTerminal::ASSIGNMENT_STATEMENT, peek().codeLocation);
    node->addChild(parseVariable());
    node->addChild(expect(TokenType::becomes));
    node->addChild(parseExpression());
    return node;
}

/**
 * Production Rule:
 * PROCEDURE-OR-FUNCTIONCALL -> ident + (lparent + PARAMETER-LIST? + rparent)
 */
CSTNodes* Parser::parseProcedureOrFunctionCall(){
    CSTNodes* node = new CSTNodes(NonTerminal::PROCEDURE_OR_FUNCTION_CALL, peek().codeLocation);
    node->addChild(expect(TokenType::ident));
    node->addChild(expect(TokenType::lparent));
    if (checkMultiple({TokenType::plus, TokenType::minus, TokenType::ident,
        TokenType::intcon, TokenType::realcon, TokenType::charcon, TokenType::string, TokenType::lparent,
        TokenType::notsy})
    ) node->addChild(parseParameterList());
    node->addChild(expect(TokenType::rparent));
    return node;
}