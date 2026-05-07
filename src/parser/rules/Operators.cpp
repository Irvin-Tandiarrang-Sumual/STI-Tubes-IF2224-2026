#include "../Parser.hpp"

/*
    RELATIONAL-OPERATOR : eql | neq | gtr | geq | lss | leq
*/
CSTNodes* Parser::parseRelationalOperator() {
    CSTNodes* node = new CSTNodes(NonTerminal::RELATIONAL_OPERATOR, peek().codeLocation);

    TokenType currentType = peek().type;
    switch (currentType) {
        case TokenType::eql:
        case TokenType::neq:
        case TokenType::gtr:
        case TokenType::geq:
        case TokenType::lss:
        case TokenType::leq:
            node->addChild(expect(currentType));
            break;
        default:
            // Kalau selain itu, fallback ke 'eql' aja (bisa yang lain sebenernya)
            node->addChild(expect(TokenType::eql)); 
            break;
    }

    return node;
}

/*
    ADDITIVE-OPERATOR : plus | minus | orsy
*/
CSTNodes* Parser::parseAdditiveOperator() {
    CSTNodes* node = new CSTNodes(NonTerminal::ADDITIVE_OPERATOR, peek().codeLocation);

    TokenType currentType = peek().type;
    switch (currentType) {
        case TokenType::plus:
        case TokenType::minus:
        case TokenType::orsy:
            node->addChild(expect(currentType));
            break;
        default:
            node->addChild(expect(TokenType::plus)); 
            break;
    }

    return node;
}

/*
    MULTIPLICATIVE-OPERATOR : times | rdiv | idiv | imod | andsy
*/
CSTNodes* Parser::parseMultiplicativeOperator() {
    CSTNodes* node = new CSTNodes(NonTerminal::MULTIPLICATIVE_OPERATOR, peek().codeLocation);

    TokenType currentType = peek().type;
    switch (currentType) {
        case TokenType::times:
        case TokenType::rdiv:
        case TokenType::idiv:
        case TokenType::imod:
        case TokenType::andsy:
            node->addChild(expect(currentType));
            break;
        default:
            node->addChild(expect(TokenType::times)); 
            break;
    }

    return node;
}