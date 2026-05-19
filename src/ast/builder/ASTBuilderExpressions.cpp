#include "../ASTBuilder.hpp"
ASTExpressionNode* ASTBuilder::buildExpression(const CSTNodes* node) {
    if (!node || node->getChildren().empty()) return nullptr;

    const auto& children = node->getChildren();
    
    // Ambil <simple-expression> bagian kiri
    auto left = buildSimpleExpression(children[0]);

    // Jika punya 3 anak, berarti ada <relational-operator> di tengah
    if (children.size() == 3) {
        const CSTNodes* opNode = children[1]->childAt(0); 
        std::string opString = operatorText(opNode->getToken().type);
        auto right = buildSimpleExpression(children[2]);
        return new ASTBinaryExpressionNode(opString, std::move(left), std::move(right));
    }

    return left;
}

ASTExpressionNode* ASTBuilder::buildSimpleExpression(const CSTNodes* node) {
    if (!node || node->getChildren().empty()) return nullptr;

    const auto& children = node->getChildren();
    size_t i = 0;
    
    bool hasLeadingSign = false;
    std::string leadingSign = "";

    // Cek apakah child pertama adalah token terminal berupa unary sign (+ atau -)
    if (children[0]->isTerminal() && (children[0]->getToken().type == TokenType::plus || children[0]->getToken().type == TokenType::minus)) {
        hasLeadingSign = true;
        leadingSign = operatorText(children[0]->getToken().type);
        i++;
    }

    auto left = buildTerm(children[i++]);

    if (hasLeadingSign) {
        left = new ASTUnaryExpressionNode(leadingSign, std::move(left));
    }

    // Looping Left-Associative untuk <additive-operator> <term>
    while (i < children.size()) {
        const CSTNodes* opNode = children[i++]->childAt(0);
        std::string opString = operatorText(opNode->getToken().type);
        auto right = buildTerm(children[i++]);
        left = new ASTBinaryExpressionNode(opString, std::move(left), std::move(right));
    }

    return left;
}

ASTExpressionNode* ASTBuilder::buildTerm(const CSTNodes* node) {
    if (!node || node->getChildren().empty()) return nullptr;

    const auto& children = node->getChildren();
    auto left = buildFactor(children[0]);
    size_t i = 1;

    // Looping Left-Associative untuk <multiplicative-operator> <factor>
    while (i < children.size()) {
        const CSTNodes* opNode = children[i++]->childAt(0);
        std::string opString = operatorText(opNode->getToken().type);
        auto right = buildFactor(children[i++]);
        left = new ASTBinaryExpressionNode(opString, std::move(left), std::move(right));
    }

    return left;
}

ASTExpressionNode* ASTBuilder::buildFactor(const CSTNodes* node) {
    if (!node || node->getChildren().empty()) return nullptr;

    const CSTNodes* child = node->childAt(0);

    if (child->isTerminal()) {
        TokenType type = child->getToken().type;
        if (type == TokenType::notsy) {
            return new ASTUnaryExpressionNode("not", buildFactor(node->childAt(1)));
        } else if (type == TokenType::lparent) {
            // Parenthesized expression: langsung ekstrak isi di dalam kurungnya
            return buildExpression(node->childAt(1));
        } else {
            return buildLiteralOrIdentifierExpression(child);
        }
    } else {
        NonTerminal nt = child->getNonTerminal();
        if (nt == NonTerminal::VARIABLE) {
            return buildVariable(child); 
        } else if (nt == NonTerminal::PROCEDURE_OR_FUNCTION_CALL) {
            return buildProcedureOrFunctionCall(child); 
        } else if (nt == NonTerminal::CONSTANT) {
            return buildConstant(child);
        }
    }

    return nullptr;
}

ASTExpressionNode* ASTBuilder::buildConstant(const CSTNodes* node) {
    if (!node || node->getChildren().empty()) return nullptr;
    const auto& children = node->getChildren();

    bool hasSign = false;
    std::string sign;
    const CSTNodes* valNode = children[0];

    // Cek unary sign di depan identifier/literal
    if (children[0]->isTerminal() && (children[0]->getToken().type == TokenType::plus || children[0]->getToken().type == TokenType::minus)) {
        hasSign = true;
        sign = operatorText(children[0]->getToken().type);
        valNode = children[1];
    }

    // Bangun literal / identifier
    auto expr = buildLiteralOrIdentifierExpression(valNode);

    if (hasSign) {
        return new ASTUnaryExpressionNode(sign, std::move(expr));
    }
    return expr;
}

ASTExpressionNode* ASTBuilder::buildLiteralOrIdentifierExpression(const CSTNodes* tokenNode) {
    const Token& token = tokenNode->getToken();
    
    switch (token.type) {
        case TokenType::intcon: {
            const std::string raw = tokenValueToString(token);
            try {
                return new ASTLiteralExpressionNode(std::stoi(raw));
            } catch (...) {
                return nullptr;
            }
        }
            
        case TokenType::realcon: {
            const std::string raw = tokenValueToString(token);
            try {
                return new ASTLiteralExpressionNode(std::stod(raw));
            } catch (...) {
                return nullptr;
            }
        }
            
        case TokenType::charcon: {
            std::string s = std::get<std::string>(token.value);
            char c = s.empty() ? '\0' : s[0];
            return new ASTLiteralExpressionNode(c);
        }
        
        case TokenType::string:
            return new ASTLiteralExpressionNode(std::get<std::string>(token.value));
            
        case TokenType::ident: {
            std::string name = std::get<std::string>(token.value);
            
            // case-insensitive btw
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            
            if (lowerName == "true") {
                return new ASTLiteralExpressionNode(true);
            } else if (lowerName == "false") {
                return new ASTLiteralExpressionNode(false);
            }
            
            return new ASTVariableExpressionNode(name, std::vector<ASTVariableComponent>{});
        }
        
        default:
            return nullptr;
    }
}