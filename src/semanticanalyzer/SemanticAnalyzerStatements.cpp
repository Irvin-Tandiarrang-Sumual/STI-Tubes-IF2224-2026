#include "SemanticAnalyzer.hpp"

std::any SemanticAnalyzer::visitAssignmentStatementNode(ASTAssignmentStatementNode* node) {
    if (node->target->components.empty()) {
        int symbolIdx = symbolTable.lookup(node->target->baseName);
        if (symbolIdx != -1 && symbolTable.getIdentifier(symbolIdx).isConstant) {
            throw std::runtime_error("Semantic Error: Tidak dapat melakukan assignment pada konstanta '" + node->target->baseName + "'.");
        }
    }
    
    DataType targetType = std::any_cast<DataType>(node->target->accept(this));
    DataType valueType = std::any_cast<DataType>(node->value->accept(this));
    ASTTypeNode* targetTypeNode = nullptr;
    if (node->target->components.empty()) {
        targetTypeNode = getStoredTypeNode(node->target->symbolRefIndex_);
    }

    ASTTypeNode* valueTypeNode = nullptr;
    if (auto* valueVar = dynamic_cast<ASTVariableExpressionNode*>(node->value)) {
        if (valueVar->components.empty()) {
            valueTypeNode = getStoredTypeNode(valueVar->symbolRefIndex_);
        }
    }

    validateAssignmentCompatibility(node->target, node->value, targetTypeNode, valueTypeNode, targetType, valueType);
    node->evalType_ = DataType::VOID;
    return {};
}

std::any SemanticAnalyzer::visitIfStatementNode(ASTIfStatementNode* node) {
    DataType condType = std::any_cast<DataType>(node->condition->accept(this));
    // Ekspresi di dalam IF-Statement HARUS bertipe Boolean
    if (condType != DataType::BOOLEAN) {
        throw std::runtime_error("Semantic Error: Kondisi di dalam IF-Statement harus menghasilkan tipe Boolean.");
    }
    safeVisitNode(node->thenBranch);
    safeVisitNode(node->elseBranch);
    node->evalType_ = DataType::VOID;
    return {};
}

std::any SemanticAnalyzer::visitBlockStatementNode(ASTBlockStatementNode* node) {
    for (auto* stmt : node->statements) {
        safeVisitNode(stmt);
    }
    node->evalType_ = DataType::VOID;
    return DataType::VOID;
}

std::any SemanticAnalyzer::visitWhileStatementNode(ASTWhileStatementNode* node) {
    DataType condType = std::any_cast<DataType>(node->condition->accept(this));
    if (condType != DataType::BOOLEAN) {
        throw std::runtime_error("Semantic Error: Kondisi di dalam WHILE-Statement harus menghasilkan tipe Boolean.");
    }
    safeVisitNode(node->body);
    node->evalType_ = DataType::VOID;
    return DataType::VOID;
}

std::any SemanticAnalyzer::visitRepeatStatementNode(ASTRepeatStatementNode* node) {
    for (auto* stmt : node->body) {
        safeVisitNode(stmt);
    }

    DataType condType = std::any_cast<DataType>(node->condition->accept(this));
    if (condType != DataType::BOOLEAN) {
        throw std::runtime_error("Semantic Error: Kondisi di dalam REPEAT-Statement harus menghasilkan tipe Boolean.");
    }
    node->evalType_ = DataType::VOID;
    return DataType::VOID;
}

std::any SemanticAnalyzer::visitForStatementNode(ASTForStatementNode* node) {
    int iteratorIdx = symbolTable.lookup(node->iteratorName);
    if (iteratorIdx == -1) {
        throw std::runtime_error("Semantic Error: Iterator '" + node->iteratorName + "' belum dideklarasikan.");
    }

    IdentifierTableEntry& iteratorEntry = symbolTable.getIdentifier(iteratorIdx);
    if (iteratorEntry.type != DataType::INTEGER) {
        throw std::runtime_error("Semantic Error: Iterator pada FOR-Statement harus bertipe Integer.");
    }

    DataType startType = std::any_cast<DataType>(node->startVal->accept(this));
    DataType endType = std::any_cast<DataType>(node->endVal->accept(this));
    if (startType != DataType::INTEGER || endType != DataType::INTEGER) {
        throw std::runtime_error("Semantic Error: Batas FOR-Statement harus bertipe Integer.");
    }

    safeVisitNode(node->body);
    node->symbolRefIndex_ = iteratorIdx;
    node->lexicalLevel_ = iteratorEntry.level;
    node->evalType_ = DataType::VOID;
    return DataType::VOID;
}

std::any SemanticAnalyzer::visitCaseStatementNode(ASTCaseStatementNode* node) {
    DataType conditionType = std::any_cast<DataType>(node->condition->accept(this));
    if (!isOrdinalKind(conditionType) && conditionType != DataType::STRING) {
        throw std::runtime_error("Semantic Error: Kondisi di dalam CASE-Statement harus ordinal/simple type.");
    }

    for (auto& branch : node->branches) {
        for (auto* constant : branch.constants) {
            DataType constantType = std::any_cast<DataType>(constant->accept(this));
            if (constantType != conditionType) {
                throw std::runtime_error("Semantic Error: Konstanta pada CASE-Statement harus bertipe sama dengan ekspresi kondisi.");
            }
        }
        safeVisitNode(branch.body);
    }

    node->evalType_ = DataType::VOID;
    return DataType::VOID;
}

std::any SemanticAnalyzer::visitCallStatementNode(ASTCallStatementNode* node) {
    if (node->callExpr == nullptr) {
        throw std::runtime_error("Semantic Error: Call statement kosong.");
    }

    int symbolIdx = symbolTable.lookup(node->callExpr->callee);
    if (symbolIdx == -1) {
        throw std::runtime_error("Semantic Error: Fungsi/prosedur '" + node->callExpr->callee + "' belum dideklarasikan.");
    }

    IdentifierTableEntry& entry = symbolTable.getIdentifier(symbolIdx);

    // Bypass buat writeln sama println
    if (node->callExpr->callee == "writeln" || node->callExpr->callee == "println") {
        for (auto* argument : node->callExpr->arguments) {
            safeVisitNode(argument);
        }
    } else {
        // Ngecek jumlah argumen seharusnya
        if (node->callExpr->arguments.size() != entry.parameterTypes.size()) {
            throw std::runtime_error("Semantic Error: Jumlah argumen untuk '" + node->callExpr->callee + "' tidak sesuai.");
        }

        for (size_t i = 0; i < node->callExpr->arguments.size(); ++i) {
            DataType argType = std::any_cast<DataType>(node->callExpr->arguments[i]->accept(this));
            if (argType != entry.parameterTypes[i]) {
                throw std::runtime_error("Semantic Error: Type Mismatch pada parameter ke-" + std::to_string(i + 1) + " saat memanggil prosedur.");
            }
        }
    }

    if (entry.type != DataType::VOID) {
        throw std::runtime_error("Semantic Error: Function call tidak boleh dipakai sebagai statement.");
    }

    node->callExpr->symbolRefIndex_ = symbolIdx;
    node->callExpr->lexicalLevel_ = entry.level;
    node->evalType_ = DataType::VOID;
    return {};
}

