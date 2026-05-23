#include "../Parser.hpp"

/*
    IF-STATEMENT : ifsy + EXPRESSION + thensy + STATEMENT + (elsesy + STATEMENT)?
*/
CSTNodes* Parser::parseIfStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::IF_STATEMENT, peek().codeLocation);
    node->addChild(expect(TokenType::ifsy));
    node->addChild(parseExpression());
    node->addChild(expect(TokenType::thensy));
    node->addChild(parseStatement());

    if (check(TokenType::elsesy)) { 
        node->addChild(expect(TokenType::elsesy));
        node->addChild(parseStatement());
    }

    return node;
}

/*
    CASE-STATEMENT : casesy + EXPRESSION + ofsy + CASE-BLOCK + endsy
*/
CSTNodes* Parser::parseCaseStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::CASE_STATEMENT, peek().codeLocation);
    node->addChild(expect(TokenType::casesy));
    node->addChild(parseExpression());
    node->addChild(expect(TokenType::ofsy));
    node->addChild(parseCaseBlock());
    node->addChild(expect(TokenType::endsy));
    return node;
}

/*
    CASE-BLOCK : CONSTANT + (comma + CONSTANT)* + colon + STATEMENT + (semicolon + CASE-BLOCK)?
*/
CSTNodes* Parser::parseCaseBlock() {
    CSTNodes* node = new CSTNodes(NonTerminal::CASE_BLOCK, peek().codeLocation);
    node->addChild(parseConstant());

    while (check(TokenType::comma)) {
        node->addChild(expect(TokenType::comma));
        node->addChild(parseConstant());
    }

    node->addChild(expect(TokenType::colon));
    node->addChild(parseStatement());

    while (check(TokenType::semicolon)) {
        node->addChild(expect(TokenType::semicolon));

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
    WHILE-STATEMENT : whilesy + EXPRESSION + dosy + COMPOUND-STATEMENT + semicolon
*/
CSTNodes* Parser::parseWhileStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::WHILE_STATEMENT, peek().codeLocation);
    node->addChild(expect(TokenType::whilesy));
    node->addChild(parseExpression());
    node->addChild(expect(TokenType::dosy));
    node->addChild(parseCompoundStatement());
    node->addChild(expect(TokenType::semicolon));
    return node;
}

/*
    REPEAT-STATEMENT : repeatsy + STATEMENT-LIST + untilsy + EXPRESSION
*/
CSTNodes* Parser::parseRepeatStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::REPEAT_STATEMENT, peek().codeLocation);
    node->addChild(expect(TokenType::repeatsy));
    node->addChild(parseStatementList());
    node->addChild(expect(TokenType::untilsy));
    node->addChild(parseExpression());
    return node;
}

/*
    FOR-STATEMENT : forsy + ident + becomes + EXPRESSION + (tosy | downtosy) + EXPRESSION + dosy + COMPOUND-STATEMENT + semicolon
*/
CSTNodes* Parser::parseForStatement() {
    CSTNodes* node = new CSTNodes(NonTerminal::FOR_STATEMENT, peek().codeLocation);
    node->addChild(expect(TokenType::forsy));
    node->addChild(expect(TokenType::ident));
    node->addChild(expect(TokenType::becomes));
    node->addChild(parseExpression());

    if (check(TokenType::tosy)) {
        node->addChild(expect(TokenType::tosy));
    } else {
        node->addChild(expect(TokenType::downtosy));
    }

    node->addChild(parseExpression());
    node->addChild(expect(TokenType::dosy));
    node->addChild(parseCompoundStatement());
    node->addChild(expect(TokenType::semicolon));
    return node;
}