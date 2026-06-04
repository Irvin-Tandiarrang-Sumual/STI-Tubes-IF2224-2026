#include "SemanticAnalyzer.hpp"

std::any SemanticAnalyzer::visitUnaryExpressionNode(ASTUnaryExpressionNode* node) {
    DataType operandType = std::any_cast<DataType>(node->operand->accept(this));

    if (node->op == "not") {
        if (operandType != DataType::BOOLEAN) {
            throw std::runtime_error("Semantic Error: Operator 'not' membutuhkan tipe Boolean.");
        }
        node->evalType_ = DataType::BOOLEAN;
        return DataType::BOOLEAN;
    }

    if (node->op == "+" || node->op == "-") {
        if (!isNumericKind(operandType) && operandType != DataType::INTEGER) {
            throw std::runtime_error("Semantic Error: Unary operator '" + node->op + "' membutuhkan tipe numerik.");
        }
        node->evalType_ = operandType;
        return operandType;
    }

    throw std::runtime_error("Semantic Error: Operator unary '" + node->op + "' tidak dikenal.");
}

std::any SemanticAnalyzer::visitVariableExpressionNode(ASTVariableExpressionNode* node) {
    int symbolIdx = symbolTable.lookup(node->baseName);
    if (symbolIdx == -1) {
        throw std::runtime_error("Semantic Error: Variabel '" + node->baseName + "' belum dideklarasikan.");
    }

    IdentifierTableEntry& entry = symbolTable.getIdentifier(symbolIdx);
    DataType currentType = entry.type;
    ASTTypeNode* currentTypeNode = getStoredTypeNode(symbolIdx);

    node->symbolRefIndex_ = symbolIdx;
    node->lexicalLevel_ = entry.level;

    for (const auto& comp : node->components) {
        if (comp.isArrayIndex) {
            auto* arrayTypeNode = dynamic_cast<ASTArrayTypeNode*>(currentTypeNode);
            if (currentType != DataType::ARRAY || arrayTypeNode == nullptr) {
                throw std::runtime_error("Semantic Error: Variabel '" + node->baseName + "' bukan berjenis Array.");
            }
            for (auto* idxExpr : comp.indices) {
                DataType idxType = std::any_cast<DataType>(idxExpr->accept(this));
                if (!isOrdinalKind(idxType) || idxType == DataType::REAL) {
                    throw std::runtime_error("Semantic Error: Indeks pengaksesan array tidak boleh bertipe Real.");
                }
            }

            currentTypeNode = resolveTypeNode(arrayTypeNode->elementType);
            currentType = resolveTypeKind(currentTypeNode);
        } else {
            auto* recordTypeNode = dynamic_cast<ASTRecordTypeNode*>(currentTypeNode);
            if (currentType != DataType::RECORD || recordTypeNode == nullptr) {
                throw std::runtime_error("Semantic Error: Variabel '" + node->baseName + "' bukan berjenis Record.");
            }

            ASTRecordFieldNode* foundField = nullptr;
            for (auto& field : recordTypeNode->fields) {
                for (const auto& identifier : field.identifiers) {
                    if (identifier == comp.fieldName) {
                        foundField = &field;
                        break;
                    }
                }
                if (foundField != nullptr) {
                    break;
                }
            }

            if (foundField == nullptr) {
                throw std::runtime_error("Semantic Error: Field record '" + comp.fieldName + "' tidak ditemukan.");
            }

            currentTypeNode = resolveTypeNode(foundField->type);
            currentType = resolveTypeKind(currentTypeNode);
        }
    }

    node->evalType_ = currentType;
    return currentType;
}

std::any SemanticAnalyzer::visitLiteralExpressionNode(ASTLiteralExpressionNode* node) {
    DataType t = std::visit([](auto&& arg) -> DataType {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>)         return DataType::INTEGER;
        else if constexpr (std::is_same_v<T, double>) return DataType::REAL;
        else if constexpr (std::is_same_v<T, char>)   return DataType::CHAR;
        else if constexpr (std::is_same_v<T, bool>)   return DataType::BOOLEAN;
        else                                          return DataType::STRING;
    }, node->value);

    node->evalType_ = t;
    return t;
}

std::any SemanticAnalyzer::visitBinaryExpressionNode(ASTBinaryExpressionNode* node) {
    DataType lhsType = std::any_cast<DataType>(node->lhs->accept(this));
    DataType rhsType = std::any_cast<DataType>(node->rhs->accept(this));
    ASTTypeNode* lhsTypeNode = dynamic_cast<ASTVariableExpressionNode*>(node->lhs) != nullptr ? getStoredTypeNode(node->lhs->symbolRefIndex_) : nullptr;
    ASTTypeNode* rhsTypeNode = dynamic_cast<ASTVariableExpressionNode*>(node->rhs) != nullptr ? getStoredTypeNode(node->rhs->symbolRefIndex_) : nullptr;
    DataType resultType = DataType::VOID;
    std::string op = node->op;

    // operator Relasional menghasilkan Boolean
    if (op == "==" || op == "<>" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "=") {
        if (!areTypesCompatible(lhsTypeNode, rhsTypeNode, lhsType, rhsType) &&
            !(isNumericKind(lhsType) && isNumericKind(rhsType)) &&
            !(lhsType == DataType::STRING && rhsType == DataType::STRING) &&
            lhsType != rhsType) {
            throw std::runtime_error("Semantic Error: Ketidakcocokan tipe data pada operasi perbandingan '" + op + "'.");
        }
        resultType = DataType::BOOLEAN;
    }

    // and/or menghasilkan Boolean
    else if (op == "and" || op == "or") {
        if (lhsType != DataType::BOOLEAN || rhsType != DataType::BOOLEAN) {
            throw std::runtime_error("Semantic Error: Operator '" + op + "' membutuhkan tipe Boolean.");
        }
        resultType = DataType::BOOLEAN;
    }
    
    // basic math
    else if (op == "+") {
        if (lhsType == DataType::INTEGER && rhsType == DataType::INTEGER) {
            resultType = DataType::INTEGER;
        } else if ((lhsType == DataType::REAL || lhsType == DataType::INTEGER) && (rhsType == DataType::REAL || rhsType == DataType::INTEGER)) {
            resultType = DataType::REAL;
        } else if (lhsType == DataType::STRING && rhsType == DataType::STRING) {
            resultType = DataType::STRING;
        } else {
            throw std::runtime_error("Semantic Error: Operator '+' memerlukan tipe numerik atau string.");
        }
    }
    else if (op == "-" || op == "*") {
        if (lhsType == DataType::INTEGER && rhsType == DataType::INTEGER) resultType = DataType::INTEGER;
        else if ((lhsType == DataType::REAL || lhsType == DataType::INTEGER) && 
                (rhsType == DataType::REAL || rhsType == DataType::INTEGER)) resultType = DataType::REAL;
        else throw std::runtime_error("Semantic Error: Operator '" + op + "' memerlukan tipe numerik.");
    }
    else if (op == "/") {
        if (!isNumericKind(lhsType) || !isNumericKind(rhsType)) {
            throw std::runtime_error("Semantic Error: Operator '/' memerlukan tipe numerik.");
        }
        resultType = DataType::REAL;
    }
    else if (op == "div" || op == "mod") {
        if (lhsType != DataType::INTEGER || rhsType != DataType::INTEGER) {
            throw std::runtime_error("Semantic Error: Operator '" + op + "' hanya boleh digunakan pada Integer.");
        }
        resultType = DataType::INTEGER;
    }
    else {
        throw std::runtime_error("Semantic Error: Operator '" + op + "' tidak dikenal.");
    }

    node->evalType_ = resultType;
    return resultType;
}

std::any SemanticAnalyzer::visitCallExpressionNode(ASTCallExpressionNode* node) {
    int symbolIdx = symbolTable.lookup(node->callee);
    if (symbolIdx == -1) {
        throw std::runtime_error("Semantic Error: Fungsi/prosedur '" + node->callee + "' belum dideklarasikan.");
    }

    IdentifierTableEntry& entry = symbolTable.getIdentifier(symbolIdx);
    if (entry.type == DataType::VOID) {
        throw std::runtime_error("Semantic Error: Prosedur '" + node->callee + "' tidak dapat digunakan sebagai ekspresi.");
    }

    if (node->arguments.size() != entry.parameterTypes.size()) {
        throw std::runtime_error("Semantic Error: Jumlah argumen untuk '" + node->callee + "' tidak sesuai. Diharapkan " + std::to_string(entry.parameterTypes.size()) + " argumen.");
    }

    for (size_t i = 0; i < node->arguments.size(); ++i) {
        DataType argType = std::any_cast<DataType>(node->arguments[i]->accept(this));
        if (argType != entry.parameterTypes[i]) {
            throw std::runtime_error("Semantic Error: Type Mismatch pada parameter ke-" + std::to_string(i + 1) + " saat memanggil '" + node->callee + "'.");
        }
    }

    node->symbolRefIndex_ = symbolIdx;
    node->lexicalLevel_ = entry.level;
    node->evalType_ = entry.type;
    return entry.type;
}

