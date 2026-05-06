#include "../Parser.hpp"

/*
    EXPRESSION : SIMPLE-EXPRESSION + (RELATIONAL-OPERATOR + SIMPLE-EXPRESSION)?
*/
CSTNodes* Parser::parseExpression() {
    CSTNodes* node = new CSTNodes(NonTerminal::EXPRESSION, peek().codeLocation);
    node->addChild(parseSimpleExpression());

    if (checkMultiple({TokenType::eql, TokenType::neq, TokenType::gtr, TokenType::geq,
                        TokenType::lss, TokenType::leq})) {
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

    if (checkMultiple({TokenType::plus, TokenType::minus})) {
        node->addChild(expect(peek().type));
    }

    node->addChild(parseTerm());

    while (checkMultiple({TokenType::plus,TokenType::minus,TokenType::orsy})) {
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

    while (checkMultiple({TokenType::times, TokenType::rdiv,
        TokenType::idiv, TokenType::imod, TokenType::andsy})) {
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
            node->addChild(expect(TokenType::lparent));
            node->addChild(parseExpression());
            node->addChild(expect(TokenType::rparent));
            break;
        case TokenType::notsy:
            node->addChild(expect(TokenType::notsy));
            node->addChild(parseFactor());
            break;
        default:
            break;
    }

    return node;
}