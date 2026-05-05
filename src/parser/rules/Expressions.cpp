#include "../Parser.hpp"

/*
    EXPRESSION : SIMPLE-EXPRESSION + (RELATIONAL-OPERATOR + SIMPLE-EXPRESSION)?
*/
CSTNodes* Parser::parseExpression() {
    CSTNodes* node = new CSTNodes(NonTerminal::EXPRESSION, peek().codeLocation);
    node->addChild(parseSimpleExpression());

    if (peek().type == TokenType::eql || peek().type == TokenType::neq ||
        peek().type == TokenType::gtr || peek().type == TokenType::geq ||
        peek().type == TokenType::lss || peek().type == TokenType::leq) {
        node->addChild(parseRelationalOperator());
        node->addChild(parseSimpleExpression());
    }

    return node;
}

/*
    SIMPLE-EXPRESSION : (plus | minus)? + TERM + (ADDITIVE-OPERATOR + TERM)*
*/
CSTNodes* Parser::parseSimpleExpression() {
    CSTNodes* node = new CSTNodes(NonTerminal::SIMPLE_EXPRESSION, peek().codeLocation);

    if (peek().type == TokenType::plus || peek().type == TokenType::minus) {
        node->addChild(new CSTNodes(expect(peek().type)));
    }

    node->addChild(parseTerm());

    while (peek().type == TokenType::plus || peek().type == TokenType::minus || peek().type == TokenType::orsy) {
        node->addChild(parseAdditiveOperator());
        node->addChild(parseTerm());
    }

    return node;
}

/*
    TERM : FACTOR + (MULTIPLICATIVE-OPERATOR + FACTOR)*
*/
CSTNodes* Parser::parseTerm() {
    CSTNodes* node = new CSTNodes(NonTerminal::TERM, peek().codeLocation);
    node->addChild(parseFactor());

    while (peek().type == TokenType::times || peek().type == TokenType::rdiv ||
           peek().type == TokenType::idiv || peek().type == TokenType::imod ||
           peek().type == TokenType::andsy) {
        node->addChild(parseMultiplicativeOperator());
        node->addChild(parseFactor());
    }

    return node;
}

/*
    FACTOR : ident | CONSTANT | VARIABLE | lparent + EXPRESSION + rparent | notsy + FACTOR
*/
CSTNodes* Parser::parseFactor() {
    CSTNodes* node = new CSTNodes(NonTerminal::FACTOR, peek().codeLocation);

    switch (peek().type) {
        case TokenType::ident:
            if (peekNext().type == TokenType::lparent) {
                node->addChild(parseProcedureOrFunctionCall());
            } else {
                node->addChild(parseVariable());
            }
            break;
        case TokenType::intcon:
        case TokenType::realcon:
        case TokenType::charcon:
        case TokenType::string:
            node->addChild(parseConstant());
            break;
        case TokenType::lparent:
            node->addChild(new CSTNodes(expect(TokenType::lparent)));
            node->addChild(parseExpression());
            node->addChild(new CSTNodes(expect(TokenType::rparent)));
            break;
        case TokenType::notsy:
            node->addChild(new CSTNodes(expect(TokenType::notsy)));
            node->addChild(parseFactor());
            break;
        default:
            break;
    }

    return node;
}