#include "../Parser.hpp"

/*
Production Rule :
    IF-STATEMENT → ifsy + EXPRESSION + thensy + STATEMENT + (elsesy + STATEMENT)?
*/
CSTNodes* Parser::parseIfStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::IF_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::ifsy)));
    node->addChild(parseExpression());
    node->addChild(new CSTNodes(expect(TokenType::thensy)));
    node->addChild(parseStatement());

    if (peek().type == TokenType::elsesy) { 
        node->addChild(new CSTNodes(expect(TokenType::elsesy)));
        node->addChild(parseStatement());
    }

    return node;
}

/*
Production Rule :
    CASE-STATEMENT → casesy + EXPRESSION + ofsy + CASE-BLOCK + endsy
*/
CSTNodes* Parser::parseCaseStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::CASE_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::casesy)));
    node->addChild(parseExpression());
    node->addChild(new CSTNodes(expect(TokenType::ofsy)));
    node->addChild(parseCaseBlock());
    node->addChild(new CSTNodes(expect(TokenType::endsy)));
    return node;
}

/*
Production Rule :
    CASE-BLOCK → CONSTANT + (comma + CONSTANT)* + colon + STATEMENT + (semicolon + CASE-BLOCK?)* 
*/
CSTNodes* Parser::parseCaseBlock() {
    CSTNodes* node = new CSTNodes(NonTerminal::CASE_BLOCK, peek().codeLocation);
    node->addChild(parseConstant());

    while (peek().type == TokenType::comma) {
        node->addChild(new CSTNodes(expect(TokenType::comma)));
        node->addChild(parseConstant());
    }

    node->addChild(new CSTNodes(expect(TokenType::colon)));
    node->addChild(parseStatement());

    while (peek().type == TokenType::semicolon) {
        node->addChild(new CSTNodes(expect(TokenType::semicolon)));

        // Ngecek dulu apakah follow(semicolon) merupakan first(CASE-BLOCK) = first(CONSTANT)
        TokenType nextType = peek().type;
        if (nextType == TokenType::charcon || nextType == TokenType::string || 
            nextType == TokenType::plus || nextType == TokenType::minus || 
            nextType == TokenType::ident || nextType == TokenType::intcon || 
            nextType == TokenType::realcon) {
    
            node->addChild(parseCaseBlock());
        }
    }
    
    return node;
}

/*
Production Rule :
    WHILE-STATEMENT → whilesy + EXPRESSION + dosy + STATEMENT
*/
CSTNodes* Parser::parseWhileStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::WHILE_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::whilesy)));
    node->addChild(parseExpression());
    node->addChild(new CSTNodes(expect(TokenType::dosy)));
    node->addChild(parseStatement());
    return node;
}

/*
Production Rule :
    REPEAT-STATEMENT → repeatsy + STATEMENT-LIST + untilsy + EXPRESSION
*/
CSTNodes* Parser::parseRepeatStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::REPEAT_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::repeatsy)));
    node->addChild(parseStatementList());
    node->addChild(new CSTNodes(expect(TokenType::untilsy)));
    node->addChild(parseExpression());
    return node;
}

/*
Production Rule :
    FOR-STATEMENT → forsy + ident + becomes + EXPRESSION + ( tosy | downtosy) + EXPRESSION + dosy + STATEMENT
*/
CSTNodes* Parser::parseForStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::FOR_STATEMENT, peek().codeLocation);
    node->addChild(new CSTNodes(expect(TokenType::forsy)));
    node->addChild(new CSTNodes(expect(TokenType::ident)));
    node->addChild(new CSTNodes(expect(TokenType::becomes)));
    node->addChild(parseExpression());

    if (peek().type == TokenType::tosy) {
        node->addChild(new CSTNodes(expect(TokenType::tosy)));
    } else {
        node->addChild(new CSTNodes(expect(TokenType::downtosy))); 
    }

    node->addChild(parseExpression());
    node->addChild(new CSTNodes(expect(TokenType::dosy)));
    node->addChild(parseStatement());
    return node;
}