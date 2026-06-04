#include "../IntermediateCode.hpp"

std::any IntermediateCodeGenerator::visitExpressionNode(ASTExpressionNode* node) {
    if (node != nullptr) {
        return visitChildren(node);
    }
    return {};
}

std::any IntermediateCodeGenerator::visitLiteralExpressionNode(ASTLiteralExpressionNode* node) {
    if (auto* strVal = std::get_if<std::string>(&node->value)) {
        this->code_.push_back(Instruction(OpCode::LIT, 0, *strVal));
    } else {
        emitLit(literalToInt(node));
    }
    return {};
}

std::any IntermediateCodeGenerator::visitVariableExpressionNode(ASTVariableExpressionNode* node) {
    if (node == nullptr) {
        return {};
    }

    if (node->components.empty()) {
        int symbolIndex;
        if(node->symbolRefIndex_ >= 0){
            symbolIndex = node->symbolRefIndex_;
        } else { 
            symbolIndex = symbolTable_.lookup(node->baseName);
        }
        
        if (symbolIndex != -1) {
            const IdentifierTableEntry& entry = symbolTable_.getIdentifier(symbolIndex);

            // Predefined boolean dan enum constant tidak punya instruksi deklarasi tersendiri.
            if (entry.isConstant && entry.obj == "constant" && node->baseName == "true") {
                emitLit(1);
                return {};
            }
            if (entry.isConstant && entry.obj == "constant" && node->baseName == "false") {
                emitLit(0);
                return {};
            }
            if (entry.isConstant && entry.obj == "constant" && entry.type == DataType::ENUMERATED) {
                emitLit(entry.address);
                return {};
            }
        }
    }

    emitLod(getRuntimeLevel(node), getRuntimeAddress(node));
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

    emitCallToSubprogram(node);
    return {};
}