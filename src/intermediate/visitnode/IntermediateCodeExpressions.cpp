#include "../IntermediateCode.hpp"

std::any IntermediateCodeGenerator::visitExpressionNode(ASTExpressionNode* node) {
    if (node != nullptr) {
        return visitChildren(node);
    }
    return {};
}

std::any IntermediateCodeGenerator::visitLiteralExpressionNode(ASTLiteralExpressionNode* node) {
    emitLit(literalToInt(node));
    return {};
}

std::any IntermediateCodeGenerator::visitVariableExpressionNode(ASTVariableExpressionNode* node) {
    if (node == nullptr) {
        return {};
    }

    emitLod(getLevelDifference(node->lexicalLevel_), getRuntimeAddress(node));
    return {};
}

std::any IntermediateCodeGenerator::visitUnaryExpressionNode(ASTUnaryExpressionNode* node) {
    if (node == nullptr || node->operand == nullptr) {
        return {};
    }

    node->operand->accept(this);

    if (node->op == "+") {
        return {};
    }

    if (node->op == "not") {
        emitLit(0);
        emitOpr(OprCode::EQL);
        return {};
    }

    emitOpr(mapUnaryOperatorToOpr(node->op));
    return {};
}

std::any IntermediateCodeGenerator::visitBinaryExpressionNode(ASTBinaryExpressionNode* node) {
    if (node == nullptr || node->lhs == nullptr || node->rhs == nullptr) {
        return {};
    }

    node->lhs->accept(this);
    node->rhs->accept(this);

    if (node->op == "and") {
        emitOpr(OprCode::MUL);
        return {};
    }

    if (node->op == "or") {
        emitOpr(OprCode::ADD);
        emitLit(0);
        emitOpr(OprCode::GTR);
        return {};
    }

    emitOpr(mapBinaryOperatorToOpr(node->op));
    return {};
}

std::any IntermediateCodeGenerator::visitCallExpressionNode(ASTCallExpressionNode* node) {
    if (node == nullptr) {
        return {};
    }

    if (isBuiltinProcedure(node->callee)) {
        throw std::runtime_error("Intermediate Error: built-in procedure '" + node->callee + "' tidak dapat digunakan sebagai expression.");
    }

    throw std::runtime_error("Intermediate Error: function call expression belum didukung oleh generator awal.");
}