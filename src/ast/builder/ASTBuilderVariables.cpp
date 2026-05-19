#include "../ASTBuilder.hpp"

ASTVariableExpressionNode* ASTBuilder::buildVariable(const CSTNodes* node) {
    if (node == nullptr || node->isError() || node->isTerminal() || node->getNonTerminal() != NonTerminal::VARIABLE) {
        return nullptr;
    }

    const CSTNodes* baseNameNode = node->firstTokenOf(TokenType::ident);
    if (baseNameNode == nullptr) {
        return nullptr;
    }

    std::vector<ASTVariableComponent> components;

    for (const CSTNodes* child : node->getChildren()) {
        if (child == nullptr || child->isError() || child->isTerminal()) {
            continue;
        }

        if (child->getNonTerminal() == NonTerminal::COMPONENT_VARIABLE) {
            components.push_back(buildComponentVariable(child));
        }
    }

    return new ASTVariableExpressionNode(tokenText(baseNameNode), std::move(components));
}

ASTVariableComponent ASTBuilder::buildComponentVariable(const CSTNodes* node) {
    ASTVariableComponent component;
    component.isArrayIndex = false;

    if (node == nullptr || node->isError() || node->isTerminal() ||
        node->getNonTerminal() != NonTerminal::COMPONENT_VARIABLE) {
        return component;
    }

    const CSTNodes* indexListNode = node->firstChildOf(NonTerminal::INDEX_LIST);
    if (indexListNode != nullptr) {
        component.isArrayIndex = true;
        component.indices = buildIndexList(indexListNode);
        return component;
    }

    const CSTNodes* fieldNameNode = node->firstTokenOf(TokenType::ident);
    if (fieldNameNode != nullptr) {
        component.isArrayIndex = false;
        component.fieldName = tokenText(fieldNameNode);
    }

    return component;
}

std::vector<ASTExpressionNode*> ASTBuilder::buildIndexList(const CSTNodes* node) {
    std::vector<ASTExpressionNode*> indices;

    if (node == nullptr || node->isError() || node->isTerminal() || node->getNonTerminal() != NonTerminal::INDEX_LIST) {
        return indices;
    }

    for (const CSTNodes* child : node->getChildren()) {
        if (child == nullptr || child->isError()) {
            continue;
        }

        if (child->isTerminal()) {
            // lbrack, rbrack, dan comma hanya pemisah sintaksis.
            continue;
        }

        if (child->getNonTerminal() == NonTerminal::EXPRESSION) {
            ASTExpressionNode* expr = buildExpression(child);
            if (expr != nullptr) {
                indices.push_back(expr);
            }
        } else if (child->getNonTerminal() == NonTerminal::INDEX_LIST) {
            auto nestedIndices = buildIndexList(child);
            for (auto* indexExpr : nestedIndices) {
                indices.push_back(indexExpr);
            }
        }
    }

    return indices;
}