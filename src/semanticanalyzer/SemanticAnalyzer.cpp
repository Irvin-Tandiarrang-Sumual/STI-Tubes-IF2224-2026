#include "SemanticAnalyzer.hpp"

void SemanticAnalyzer::reportError(const std::string& message) {
    errors_.push_back(message);
}

SymbolTable& SemanticAnalyzer::getSymbolTable() {
    return symbolTable;
}

ASTTypeNode* SemanticAnalyzer::getResolvedTypeNode(ASTTypeNode* node) {
    return resolveTypeNode(node);
}

ASTTypeNode* SemanticAnalyzer::getIdentifierTypeNode(int symbolIndex) {
    return getStoredTypeNode(symbolIndex);
}

int SemanticAnalyzer::getTypeStorageSize(ASTTypeNode* typeNode) {
    return estimateTypeStorageSize(typeNode);
}

bool SemanticAnalyzer::getRangeBounds(const ASTRangeType* rangeNode, int& low, int& high) const {
    return extractRangeBounds(rangeNode, low, high);
}

DataType SemanticAnalyzer::getResolvedTypeKind(ASTTypeNode* node) {
    return resolveTypeKind(node);
}

void SemanticAnalyzer::reportError(const ASTNode* node, const std::string& message) {
    if (node == nullptr) {
        errors_.push_back(message);
        return;
    }

    const auto line = node->location_.line;
    const auto col = node->location_.col;
    errors_.push_back("[line " + std::to_string(line) + ", col " + std::to_string(col) + "] " + message);
}

void SemanticAnalyzer::reportWarning(const std::string& message) {
    warnings_.push_back(message);
}

void SemanticAnalyzer::reportWarning(const ASTNode* node, const std::string& message) {
    if (node == nullptr) {
        reportWarning(message);
        return;
    }

    const auto line = node->location_.line;
    const auto col = node->location_.col;

    warnings_.push_back(
        "[line " + std::to_string(line) +
        ", col " + std::to_string(col) + "] " +
        "Warning: " + message
    );
}

const std::vector<std::string>& SemanticAnalyzer::getWarnings() const {
    return warnings_;
}

bool SemanticAnalyzer::extractConstantNumber(ASTExpressionNode* expression, double& outValue) {
    if (expression == nullptr) {
        return false;
    }

    auto* literal = dynamic_cast<ASTLiteralExpressionNode*>(expression);
    if (literal == nullptr) {
        return false;
    }

    if (std::holds_alternative<int>(literal->value)) {
        outValue = static_cast<double>(std::get<int>(literal->value));
        return true;
    }

    if (std::holds_alternative<double>(literal->value)) {
        outValue = std::get<double>(literal->value);
        return true;
    }

    return false;
}

bool SemanticAnalyzer::extractConstantBoolean(ASTExpressionNode* expression, bool& outValue) {
    if (expression == nullptr) {
        return false;
    }

    auto* literal = dynamic_cast<ASTLiteralExpressionNode*>(expression);
    if (literal == nullptr) {
        return false;
    }

    if (std::holds_alternative<bool>(literal->value)) {
        outValue = std::get<bool>(literal->value);
        return true;
    }

    return false;
}

SemanticAnalyzer::ConstantBoolResult SemanticAnalyzer::evaluateConstantBoolean(ASTExpressionNode* expression) {
    if (expression == nullptr) {
        return ConstantBoolResult::Unknown;
    }

    bool boolValue = false;
    if (extractConstantBoolean(expression, boolValue)) {
        return boolValue ? ConstantBoolResult::AlwaysTrue : ConstantBoolResult::AlwaysFalse;
    }

    auto* unary = dynamic_cast<ASTUnaryExpressionNode*>(expression);
    if (unary != nullptr && unary->op == "not") {
        ConstantBoolResult operand = evaluateConstantBoolean(unary->operand);

        if (operand == ConstantBoolResult::AlwaysTrue) {
            return ConstantBoolResult::AlwaysFalse;
        }

        if (operand == ConstantBoolResult::AlwaysFalse) {
            return ConstantBoolResult::AlwaysTrue;
        }

        return ConstantBoolResult::Unknown;
    }

    auto* binary = dynamic_cast<ASTBinaryExpressionNode*>(expression);
    if (binary == nullptr) {
        return ConstantBoolResult::Unknown;
    }

    const std::string& op = binary->op;

    if (op == "and" || op == "or") {
        ConstantBoolResult left = evaluateConstantBoolean(binary->lhs);
        ConstantBoolResult right = evaluateConstantBoolean(binary->rhs);

        if (op == "and") {
            if (left == ConstantBoolResult::AlwaysFalse || right == ConstantBoolResult::AlwaysFalse) {
                return ConstantBoolResult::AlwaysFalse;
            }

            if (left == ConstantBoolResult::AlwaysTrue && right == ConstantBoolResult::AlwaysTrue) {
                return ConstantBoolResult::AlwaysTrue;
            }
        }

        if (op == "or") {
            if (left == ConstantBoolResult::AlwaysTrue || right == ConstantBoolResult::AlwaysTrue) {
                return ConstantBoolResult::AlwaysTrue;
            }

            if (left == ConstantBoolResult::AlwaysFalse && right == ConstantBoolResult::AlwaysFalse) {
                return ConstantBoolResult::AlwaysFalse;
            }
        }

        return ConstantBoolResult::Unknown;
    }

    if (op == "==" || op == "<>" || op == ">" || op == ">=" || op == "<" || op == "<=") {
        double leftNumber = 0;
        double rightNumber = 0;

        if (!extractConstantNumber(binary->lhs, leftNumber) ||
            !extractConstantNumber(binary->rhs, rightNumber)) {
            return ConstantBoolResult::Unknown;
        }

        bool result = false;

        if (op == "==") {
            result = leftNumber == rightNumber;
        } else if (op == "<>") {
            result = leftNumber != rightNumber;
        } else if (op == ">") {
            result = leftNumber > rightNumber;
        } else if (op == ">=") {
            result = leftNumber >= rightNumber;
        } else if (op == "<") {
            result = leftNumber < rightNumber;
        } else if (op == "<=") {
            result = leftNumber <= rightNumber;
        }

        return result ? ConstantBoolResult::AlwaysTrue : ConstantBoolResult::AlwaysFalse;
    }

    return ConstantBoolResult::Unknown;
}

void SemanticAnalyzer::safeVisitNode(ASTNode* node) {
    if (node == nullptr) {
        return;
    }
    try {
        node->accept(this);
    } catch (const std::exception& ex) {
        reportError(node, ex.what());
    }
}

void SemanticAnalyzer::setIdentifierObject(int symbolIndex, const std::string& objectClass) {
    symbolTable.getIdentifier(symbolIndex).obj = objectClass;
}

DataType SemanticAnalyzer::mapStringToDataType(const std::string& typeStr) {
    std::string lowerStr = typeStr;
    for (auto &c : lowerStr) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    if (lowerStr == "integer") return DataType::INTEGER;
    if (lowerStr == "real")    return DataType::REAL;
    if (lowerStr == "char")    return DataType::CHAR;
    if (lowerStr == "boolean") return DataType::BOOLEAN;
    if (lowerStr == "string")  return DataType::STRING;
    throw std::runtime_error("Semantic Error: Tipe data '" + typeStr + "' tidak dikenal.");
}

ASTTypeNode* SemanticAnalyzer::resolveTypeNode(ASTTypeNode* node) {
    if (node == nullptr) {
        return nullptr;
    }

    auto* namedTypeNode = dynamic_cast<ASTNamedTypeNode*>(node);
    if (namedTypeNode == nullptr) {
        return node;
    }

    auto it = namedTypeDefinitions_.find(namedTypeNode->typeName);
    if (it == namedTypeDefinitions_.end() || it->second == nullptr || it->second == node) {
        return node;
    }

    return resolveTypeNode(it->second);
}

DataType SemanticAnalyzer::resolveTypeKind(ASTTypeNode* node) {
    ASTTypeNode* resolved = resolveTypeNode(node);
    if (resolved == nullptr) {
        return DataType::VOID;
    }

    if (auto* primitive = dynamic_cast<ASTPrimitiveType*>(resolved)) {
        return mapStringToDataType(primitive->type);
    }
    if (dynamic_cast<ASTRangeType*>(resolved) != nullptr) {
        return DataType::RANGE;
    }
    if (dynamic_cast<ASTArrayTypeNode*>(resolved) != nullptr) {
        return DataType::ARRAY;
    }
    if (dynamic_cast<ASTRecordTypeNode*>(resolved) != nullptr) {
        return DataType::RECORD;
    }
    if (dynamic_cast<ASTEnumeratedTypeNode*>(resolved) != nullptr) {
        return DataType::ENUMERATED;
    }
    if (dynamic_cast<ASTNamedTypeNode*>(resolved) != nullptr) {
        return DataType::VOID;
    }
    return DataType::VOID;
}

ASTTypeNode* SemanticAnalyzer::getStoredTypeNode(int symbolIndex) {
    auto it = identifierTypeNodes_.find(symbolIndex);
    if (it == identifierTypeNodes_.end()) {
        return nullptr;
    }
    return resolveTypeNode(it->second);
}

DataType SemanticAnalyzer::resolveExpressionKind(ASTExpressionNode* expression) {
    if (expression == nullptr) {
        return DataType::VOID;
    }
    return expression->evalType_;
}

DataType SemanticAnalyzer::resolveTypeNameKind(const std::string& typeName) {
    int typeIdx = symbolTable.lookup(typeName);
    if (typeIdx == -1) {
        throw std::runtime_error("Semantic Error: Tipe '" + typeName + "' belum dideklarasikan.");
    }
    return symbolTable.getIdentifier(typeIdx).type;
}

void SemanticAnalyzer::rememberIdentifierType(int symbolIndex, ASTTypeNode* typeNode) {
    identifierTypeNodes_[symbolIndex] = resolveTypeNode(typeNode);
}

int SemanticAnalyzer::literalToInt(const ASTLiteralExpressionNode* literal) const {
    if (literal == nullptr) {
        return 0;
    }

    return std::visit([](const auto& value) -> int {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, bool>) {
            return value ? 1 : 0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return 0;
        } else {
            return static_cast<int>(value);
        }
    }, literal->value);
}

bool SemanticAnalyzer::extractRangeBounds(const ASTRangeType* range, int& low, int& high) const {
    if (range == nullptr) {
        return false;
    }

    auto it = rangeBounds_.find(range);
    if (it != rangeBounds_.end()) {
        low = std::visit([](const auto& value) -> int {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, bool>) {
                return value ? 1 : 0;
            } else {
                return static_cast<int>(value);
            }
        }, it->second.first);
        high = std::visit([](const auto& value) -> int {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, bool>) {
                return value ? 1 : 0;
            } else {
                return static_cast<int>(value);
            }
        }, it->second.second);
        return true;
    }

    auto* startLit = dynamic_cast<ASTLiteralExpressionNode*>(range->startConstant);
    auto* endLit = dynamic_cast<ASTLiteralExpressionNode*>(range->endConstant);
    if (startLit == nullptr || endLit == nullptr) {
        return false;
    }

    low = literalToInt(startLit);
    high = literalToInt(endLit);
    return true;
}

int SemanticAnalyzer::estimateTypeStorageSize(ASTTypeNode* typeNode) {
    ASTTypeNode* resolved = resolveTypeNode(typeNode);
    if (resolved == nullptr) {
        return 1;
    }

    if (dynamic_cast<ASTPrimitiveType*>(resolved) != nullptr) {
        return 1;
    }

    if (dynamic_cast<ASTRangeType*>(resolved) != nullptr ||
        dynamic_cast<ASTEnumeratedTypeNode*>(resolved) != nullptr) {
        return 1;
    }

    if (auto* arrayType = dynamic_cast<ASTArrayTypeNode*>(resolved)) {
        int arrayRef = ensureArrayTypeEntry(arrayType);
        if (arrayRef >= 0) {
            return symbolTable.getArrayEntry(arrayRef).size;
        }
        return 1;
    }

    if (auto* recordType = dynamic_cast<ASTRecordTypeNode*>(resolved)) {
        int totalSize = 0;
        for (const auto& field : recordType->fields) {
            int fieldSize = estimateTypeStorageSize(field.type);
            totalSize += static_cast<int>(field.identifiers.size()) * fieldSize;
        }
        return totalSize > 0 ? totalSize : 1;
    }

    return 1;
}

int SemanticAnalyzer::ensureArrayTypeEntry(ASTArrayTypeNode* node) {
    if (node == nullptr) {
        return -1;
    }

    auto it = arrayTypeEntries_.find(node);
    if (it != arrayTypeEntries_.end()) {
        return it->second;
    }

    ASTTypeNode* indexTypeNode = resolveTypeNode(node->indexType);
    ASTTypeNode* elementTypeNode = resolveTypeNode(node->elementType);

    DataType indexKind = resolveTypeKind(indexTypeNode);
    if (!isOrdinalKind(indexKind) || indexKind == DataType::REAL) {
        throw std::runtime_error("Semantic Error: Index type dari Array tidak boleh bertipe Real.");
    }

    int low = 0;
    int high = 0;
    if (!extractRangeBounds(dynamic_cast<ASTRangeType*>(indexTypeNode), low, high)) {
        low = 0;
        high = 0;
    }

    int compositeRef = 0;
    int elementSize = estimateTypeStorageSize(elementTypeNode);
    if (auto* nestedArray = dynamic_cast<ASTArrayTypeNode*>(resolveTypeNode(node->elementType))) {
        compositeRef = ensureArrayTypeEntry(nestedArray);
        if (compositeRef >= 0) {
            elementSize = symbolTable.getArrayEntry(compositeRef).size;
        }
    } else if (dynamic_cast<ASTRecordTypeNode*>(elementTypeNode) != nullptr) {
        if (auto* namedElementType = dynamic_cast<ASTNamedTypeNode*>(node->elementType)) {
            int typeIdx = symbolTable.lookup(namedElementType->typeName);
            if (typeIdx >= 0) {
                compositeRef = typeIdx;
            }
        }
    }

    DataType elementKind = resolveTypeKind(elementTypeNode);
    int totalSize = (high - low + 1) * elementSize;
    int arrayRef = symbolTable.insertArray(indexKind, elementKind, compositeRef, low, high, elementSize, totalSize);
    arrayTypeEntries_[node] = arrayRef;
    return arrayRef;
}

bool SemanticAnalyzer::isNumericKind(DataType type) const {
    return type == DataType::INTEGER || type == DataType::REAL;
}

bool SemanticAnalyzer::isOrdinalKind(DataType type) const {
    return type == DataType::INTEGER || type == DataType::CHAR || type == DataType::BOOLEAN ||
           type == DataType::RANGE || type == DataType::ENUMERATED;
}

bool SemanticAnalyzer::compareRangeLiteral(const ASTLiteralExpressionNode* literal, const ASTRangeType* range) const {
    auto it = rangeBounds_.find(range);
    if (it == rangeBounds_.end()) {
        return true;
    }

    const auto& [lowBound, highBound] = it->second;
    return std::visit([&](const auto& value) -> bool {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            if (auto low = std::get_if<int>(&lowBound)) {
                if (auto high = std::get_if<int>(&highBound)) {
                    return value >= *low && value <= *high;
                }
            }
            return true;
        } else if constexpr (std::is_same_v<T, char>) {
            if (auto low = std::get_if<char>(&lowBound)) {
                if (auto high = std::get_if<char>(&highBound)) {
                    return value >= *low && value <= *high;
                }
            }
            return true;
        } else if constexpr (std::is_same_v<T, bool>) {
            if (auto low = std::get_if<bool>(&lowBound)) {
                if (auto high = std::get_if<bool>(&highBound)) {
                    return value >= *low && value <= *high;
                }
            }
            return true;
        } else {
            return true;
        }
    }, literal->value);
}

bool SemanticAnalyzer::areNamedRecordsCompatible(ASTTypeNode* lhsNode, ASTTypeNode* rhsNode, const IdentifierTableEntry& lhsEntry, const IdentifierTableEntry& rhsEntry) const {
    if (lhsEntry.typeName.empty() || rhsEntry.typeName.empty()) {
        return false;
    }
    return lhsEntry.typeName == rhsEntry.typeName && lhsNode != nullptr && rhsNode != nullptr;
}

bool SemanticAnalyzer::areTypesCompatible(ASTTypeNode* lhsNode, ASTTypeNode* rhsNode, DataType lhsKind, DataType rhsKind) {
    if (lhsNode != nullptr) {
        lhsKind = resolveTypeKind(lhsNode);
    }

    if (rhsNode != nullptr) {
        rhsKind = resolveTypeKind(rhsNode);
    }

    if (lhsKind == rhsKind) {
        if (lhsKind == DataType::RECORD) {
            auto* lhsRecord = dynamic_cast<ASTRecordTypeNode*>(lhsNode);
            auto* rhsRecord = dynamic_cast<ASTRecordTypeNode*>(rhsNode);

            if (lhsRecord == nullptr || rhsRecord == nullptr) {
                return false;
            }

            if (lhsRecord->isAnonymous || rhsRecord->isAnonymous) {
                return lhsRecord == rhsRecord;
            }

            return lhsRecord == rhsRecord;
        }

        if (lhsKind == DataType::ARRAY) {
            auto* lhsArray = dynamic_cast<ASTArrayTypeNode*>(lhsNode);
            auto* rhsArray = dynamic_cast<ASTArrayTypeNode*>(rhsNode);
            if (lhsArray == nullptr || rhsArray == nullptr) {
                return false;
            }
            return areTypesCompatible(lhsArray->indexType, rhsArray->indexType,
                                      resolveTypeKind(lhsArray->indexType), resolveTypeKind(rhsArray->indexType)) &&
                   areTypesCompatible(lhsArray->elementType, rhsArray->elementType,
                                      resolveTypeKind(lhsArray->elementType), resolveTypeKind(rhsArray->elementType));
        }

        return true;
    }

    if (lhsKind == DataType::RANGE || rhsKind == DataType::RANGE) {
        if (lhsKind == DataType::RANGE && rhsKind == DataType::RANGE) {
            auto* lhsRange = dynamic_cast<ASTRangeType*>(lhsNode);
            auto* rhsRange = dynamic_cast<ASTRangeType*>(rhsNode);
            if (lhsRange == nullptr || rhsRange == nullptr) {
                return false;
            }
            return resolveExpressionKind(lhsRange->startConstant) == resolveExpressionKind(rhsRange->startConstant) &&
                   resolveExpressionKind(lhsRange->endConstant) == resolveExpressionKind(rhsRange->endConstant);
        }

        const ASTRangeType* rangeNode = dynamic_cast<ASTRangeType*>(lhsKind == DataType::RANGE ? lhsNode : rhsNode);
        DataType otherKind = lhsKind == DataType::RANGE ? rhsKind : lhsKind;
        if (rangeNode == nullptr) {
            return false;
        }
        return otherKind == resolveExpressionKind(rangeNode->startConstant);
    }

    if (lhsKind == DataType::STRING && rhsKind == DataType::STRING) {
        return true;
    }

    return false;
}

bool SemanticAnalyzer::isInsideFunction() const {
    return !functionStack_.empty();
}

std::string SemanticAnalyzer::currentFunctionName() const {
    if (functionStack_.empty()) {
        return "";
    }

    return functionStack_.back();
}

bool SemanticAnalyzer::isFunctionReturnAssignment(ASTAssignmentStatementNode* node) const {
    if (node == nullptr || node->target == nullptr) {
        return false;
    }

    if (!isInsideFunction()) {
        return false;
    }

    return node->target->baseName == currentFunctionName();
}

SemanticAnalyzer::FlowResult SemanticAnalyzer::analyzeStatementFlow(ASTStatementNode* statement) {
    if (statement == nullptr) {
        return FlowResult{false};
    }

    if (auto* assign = dynamic_cast<ASTAssignmentStatementNode*>(statement)) {
        if (isFunctionReturnAssignment(assign)) {
            return FlowResult{true};
        }

        return FlowResult{false};
    }

    if (auto* block = dynamic_cast<ASTBlockStatementNode*>(statement)) {
        return analyzeBlockFlow(block);
    }

    if (auto* ifStmt = dynamic_cast<ASTIfStatementNode*>(statement)) {
        FlowResult thenFlow = analyzeStatementFlow(ifStmt->thenBranch);
        FlowResult elseFlow = analyzeStatementFlow(ifStmt->elseBranch);

        // IF dianggap always return hanya jika then dan else sama-sama return
        if (ifStmt->elseBranch != nullptr &&
            thenFlow.alwaysReturns &&
            elseFlow.alwaysReturns) {
            return FlowResult{true};
        }

        return FlowResult{false};
    }

    // while/repeat/for tidak dianggap pasti return
    return FlowResult{false};
}

SemanticAnalyzer::FlowResult SemanticAnalyzer::analyzeBlockFlow(ASTBlockStatementNode* block) {
    if (block == nullptr) {
        return FlowResult{false};
    }

    bool alreadyReturned = false;

    for (ASTStatementNode* statement : block->statements) {
        FlowResult statementFlow = analyzeStatementFlow(statement);

        if (statementFlow.alwaysReturns) {
            alreadyReturned = true;
        }
    }

    return FlowResult{alreadyReturned};
}

SemanticAnalyzer::SemanticAnalyzer() {
    initializePredefinedIdentifiers();
}

void SemanticAnalyzer::analyze(ASTProgramNode* root) {
    errors_.clear();
    warnings_.clear();
    safeVisitNode(root);
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors_;
}

bool SemanticAnalyzer::hasErrors() const {
    return !errors_.empty();
}

std::string SemanticAnalyzer::dumpTables() const {
    return symbolTable.dumpTables();
}

void SemanticAnalyzer::validateAssignmentCompatibility(ASTExpressionNode* target, ASTExpressionNode* value, ASTTypeNode* targetTypeNode, ASTTypeNode* valueTypeNode, DataType targetType, DataType valueType) {
    (void)target;
    (void)value;

    targetTypeNode = resolveTypeNode(targetTypeNode);
    valueTypeNode = resolveTypeNode(valueTypeNode);

    if (targetTypeNode != nullptr) {
        targetType = resolveTypeKind(targetTypeNode);
    }

    if (valueTypeNode != nullptr) {
        valueType = resolveTypeKind(valueTypeNode);
    }

    if (targetType == DataType::REAL && valueType == DataType::INTEGER) {
        return;
    }

    if (targetType == DataType::STRING && valueType == DataType::STRING) {
        return;
    }

    if (areTypesCompatible(targetTypeNode, valueTypeNode, targetType, valueType)) {
        return;
    }

    throw std::runtime_error("Semantic Error: Tipe assignment tidak kompatibel.");
}

