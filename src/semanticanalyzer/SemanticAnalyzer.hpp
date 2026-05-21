#pragma once
#include <stdexcept>
#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <any>

#include "../ast/ASTVisitor.hpp"
#include "../symboltable/Entries.hpp"
#include "../symboltable/SymbolTable.hpp"
#include "../ast/ASTNodes.hpp"

class SemanticAnalyzer : public ASTVisitor {
    private:
        SymbolTable symbolTable;
        int currentLevel = 0;
        std::unordered_map<std::string, ASTTypeNode*> namedTypeDefinitions_;
        std::unordered_map<int, ASTTypeNode*> identifierTypeNodes_;

        using RangeBound = std::variant<int, char, bool>;
        std::unordered_map<const ASTRangeType*, std::pair<RangeBound, RangeBound>> rangeBounds_;

        DataType mapStringToDataType(const std::string& typeStr) {
            std::string lowerStr = typeStr;
            for (auto &c : lowerStr) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

            if (lowerStr == "integer") return DataType::INTEGER;
            if (lowerStr == "real")    return DataType::REAL;
            if (lowerStr == "char")    return DataType::CHAR;
            if (lowerStr == "boolean") return DataType::BOOLEAN;
            if (lowerStr == "string")  return DataType::STRING;
            throw std::runtime_error("Semantic Error: Tipe data '" + typeStr + "' tidak dikenal.");
        }

        ASTTypeNode* resolveTypeNode(ASTTypeNode* node) {
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

        DataType resolveTypeKind(ASTTypeNode* node) {
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

        ASTTypeNode* getStoredTypeNode(int symbolIndex) {
            auto it = identifierTypeNodes_.find(symbolIndex);
            if (it == identifierTypeNodes_.end()) {
                return nullptr;
            }
            return resolveTypeNode(it->second);
        }

        DataType resolveExpressionKind(ASTExpressionNode* expression) {
            if (expression == nullptr) {
                return DataType::VOID;
            }
            return expression->evalType_;
        }

        DataType resolveTypeNameKind(const std::string& typeName) {
            int typeIdx = symbolTable.lookup(typeName);
            if (typeIdx == -1) {
                throw std::runtime_error("Semantic Error: Tipe '" + typeName + "' belum dideklarasikan.");
            }
            return symbolTable.getIdentifier(typeIdx).type;
        }

        void rememberIdentifierType(int symbolIndex, ASTTypeNode* typeNode) {
            identifierTypeNodes_[symbolIndex] = resolveTypeNode(typeNode);
        }

        bool isNumericKind(DataType type) const {
            return type == DataType::INTEGER || type == DataType::REAL;
        }

        bool isOrdinalKind(DataType type) const {
            return type == DataType::INTEGER || type == DataType::CHAR || type == DataType::BOOLEAN ||
                   type == DataType::RANGE || type == DataType::ENUMERATED;
        }

        bool compareRangeLiteral(const ASTLiteralExpressionNode* literal, const ASTRangeType* range) const {
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

        bool areNamedRecordsCompatible(ASTTypeNode* lhsNode, ASTTypeNode* rhsNode, const IdentifierTableEntry& lhsEntry, const IdentifierTableEntry& rhsEntry) const {
            if (lhsEntry.typeName.empty() || rhsEntry.typeName.empty()) {
                return false;
            }
            return lhsEntry.typeName == rhsEntry.typeName && lhsNode != nullptr && rhsNode != nullptr;
        }

        bool areTypesCompatible(ASTTypeNode* lhsNode, ASTTypeNode* rhsNode, DataType lhsKind, DataType rhsKind) {
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

        // init predefined
        void initializePredefinedIdentifiers() {
            // dummy
            symbolTable.insertVariable("<dummy>", DataType::VOID);

            // (idx : 1-27)
            std::vector<std::string> keywords = {
                "and", "array", "begin", "case", "const", "div", "downto", "do", 
                "else", "end", "for", "function", "if", "mod", "not", "of", 
                "or", "procedure", "program", "record", "repeat", "then", "to", 
                "type", "until", "var", "while"
            };
            for (const auto& kw : keywords) {
                symbolTable.insertVariable(kw, DataType::VOID);
            }

            // predefined Types (Indeks 28 - 32)
            symbolTable.insertVariable("integer", DataType::INTEGER);
            symbolTable.insertVariable("real",    DataType::REAL);
            symbolTable.insertVariable("char",    DataType::CHAR);
            symbolTable.insertVariable("boolean", DataType::BOOLEAN);
            symbolTable.insertVariable("string",  DataType::STRING);
            // additional
            symbolTable.insertVariable("writeln", DataType::VOID);
            symbolTable.insertVariable("println",  DataType::VOID);
        }

        // Sebuah type T2 adalah assignment-compatible dengan type T1
        // jika memenuhi salah satu dari kondisi berikut:
        void validateAssignmentCompatibility(ASTExpressionNode* target, ASTExpressionNode* value,
                                            ASTTypeNode* targetTypeNode, ASTTypeNode* valueTypeNode,
                                            DataType T1, DataType T2) {
            targetTypeNode = resolveTypeNode(targetTypeNode);
            valueTypeNode = resolveTypeNode(valueTypeNode);

            if (targetTypeNode != nullptr) {
                T1 = resolveTypeKind(targetTypeNode);
            }

            if (valueTypeNode != nullptr) {
                T2 = resolveTypeKind(valueTypeNode);
            }

            // T1 adalah Real dan T2 adalah Integer
            if (T1 == DataType::REAL && T2 == DataType::INTEGER) {
                return; 
            }

            // T1 and T2 adalah compatible dan bertipe String
            if (T1 == DataType::STRING && T2 == DataType::STRING) {
                return;
            }

            if (T1 == DataType::RANGE) {
                auto* rangeNode = dynamic_cast<ASTRangeType*>(targetTypeNode);
                if (rangeNode != nullptr) {
                    if (auto* literalValue = dynamic_cast<ASTLiteralExpressionNode*>(value)) {
                        if (!compareRangeLiteral(literalValue, rangeNode)) {
                            throw std::runtime_error("Semantic Error: Nilai literal berada di luar batas subrange.");
                        }
                        return;
                    }

                    if (T2 == resolveExpressionKind(rangeNode->startConstant)) {
                        return;
                    }
                }
            }

            if (T1 == DataType::RECORD && T2 == DataType::RECORD) {
                auto* varTarget = dynamic_cast<ASTVariableExpressionNode*>(target);
                auto* varValue = dynamic_cast<ASTVariableExpressionNode*>(value);

                if (varTarget && varValue) {
                    auto& entryTarget = symbolTable.getIdentifier(varTarget->symbolRefIndex_);
                    auto& entryValue = symbolTable.getIdentifier(varValue->symbolRefIndex_);

                    if (!areNamedRecordsCompatible(targetTypeNode, valueTypeNode, entryTarget, entryValue)) {
                        throw std::runtime_error("Semantic Error: Incompatible Record Assignment! Dua anonymous record atau record dengan tipe berbeda tidak dapat dilakukan assignment.");
                    }
                    return;
                }
            }

            if (T1 == DataType::ARRAY && T2 == DataType::ARRAY) {
                if (areTypesCompatible(targetTypeNode, valueTypeNode, T1, T2)) {
                    return;
                }
            }

            // T1 dan T2 adalah tipe primitif/simple type yang sama (Integer, Boolean, Char)
            if (T1 == T2) {
                return;
            }

            throw std::runtime_error("Semantic Error: Type Mismatch! Target dan ekspresi nilai tidak Assignment-Compatible.");
        }

    public:
        // Constructor otomatis memanggil inisialisasi Predefined Identifiers
        SemanticAnalyzer() {
            initializePredefinedIdentifiers();
        }

        // root
        std::any visitProgramNode(ASTProgramNode* node) override {
            // tar printout di luar
            std::cout << "Menganalisis Program: " << node->programName << std::endl;
            if (node->declarations != nullptr) node->declarations->accept(this);
            if (node->mainBlock != nullptr) node->mainBlock->accept(this);
            node->evalType_ = DataType::VOID;
            return {};
        }

        std::any visitDeclarationsNode(ASTDeclarationsNode* node) override {
            for (auto* decl : node->declarations) {
                if (decl != nullptr) decl->accept(this);
            }
            node->evalType_ = DataType::VOID;
            return {};
        }

        // variabel & tipe
        std::any visitTypeDeclarationNode(ASTTypeDeclarationNode* node) override {
            if (node->typeDefinition == nullptr) {
                throw std::runtime_error("Semantic Error: Definisi tipe '" + node->name + "' kosong.");
            }

            DataType typeKind = std::any_cast<DataType>(node->typeDefinition->accept(this));

            int existingIdx = symbolTable.lookup(node->name);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Tipe '" + node->name + "' sudah dideklarasikan di scope ini!");
            }

            int refIdx = symbolTable.insertVariable(node->name, typeKind);
            IdentifierTableEntry& entry = symbolTable.getIdentifier(refIdx);
            entry.typeName = node->name;

            if (auto* recordType = dynamic_cast<ASTRecordTypeNode*>(node->typeDefinition)) {
                recordType->isAnonymous = false;
            }
            node->typeDefinition->isAnonymous = false;

            namedTypeDefinitions_[node->name] = node->typeDefinition;
            rememberIdentifierType(refIdx, node->typeDefinition);

            node->symbolRefIndex_ = refIdx;
            node->lexicalLevel_ = currentLevel;
            node->evalType_ = typeKind;
            return typeKind;
        }

        std::any visitConstDeclarationNode(ASTConstDeclarationNode* node) override {
            if (node->value == nullptr) {
                throw std::runtime_error("Semantic Error: Konstanta '" + node->name + "' tidak memiliki nilai.");
            }

            DataType constType = std::any_cast<DataType>(node->value->accept(this));

            int existingIdx = symbolTable.lookup(node->name);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Konstanta '" + node->name + "' sudah dideklarasikan di scope ini!");
            }

            int refIdx = symbolTable.insertVariable(node->name, constType);
            IdentifierTableEntry& entry = symbolTable.getIdentifier(refIdx);
            entry.normal = true;

            node->symbolRefIndex_ = refIdx;
            node->lexicalLevel_ = currentLevel;
            node->evalType_ = constType;
            return constType;
        }

        std::any visitVarDeclarationNode(ASTVarDeclarationNode* node) override {
            DataType varType = DataType::INTEGER;
            std::string registeredTypeName = "";
            ASTTypeNode* resolvedTypeNode = nullptr;

            if (node->type != nullptr) {
                varType = std::any_cast<DataType>(node->type->accept(this));
                resolvedTypeNode = resolveTypeNode(node->type);
                
                // Cek apakah tipenya menggunakan NamedType (punya nama alias)
                auto* namedTypeNode = dynamic_cast<ASTNamedTypeNode*>(node->type);
                if (namedTypeNode) {
                    registeredTypeName = namedTypeNode->typeName;
                }
            }

            std::string varName = node->identifiers; 

            int existingIdx = symbolTable.lookup(varName);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Variabel '" + varName + "' sudah dideklarasikan di scope ini!");
            }

            int refIdx = symbolTable.insertVariable(varName, varType);
            
            symbolTable.getIdentifier(refIdx).typeName = registeredTypeName;
            rememberIdentifierType(refIdx, resolvedTypeNode != nullptr ? resolvedTypeNode : node->type);

            node->symbolRefIndex_ = refIdx;
            node->lexicalLevel_ = currentLevel;
            node->evalType_ = varType;

            return {};
        }

        std::any visitPrimitiveType(ASTPrimitiveType* node) override {
            DataType t = mapStringToDataType(node->type);
            node->evalType_ = t;
            node->isAnonymous = true;
            return t;
        }

        std::any visitNamedTypeNode(ASTNamedTypeNode* node) override {
            int typeIdx = symbolTable.lookup(node->typeName);
            if (typeIdx == -1) {
                throw std::runtime_error("Semantic Error: Tipe '" + node->typeName + "' belum dideklarasikan.");
            }
            DataType t = symbolTable.getIdentifier(typeIdx).type;
            node->evalType_ = t;
            auto storedTypeNode = getStoredTypeNode(typeIdx);
            if (storedTypeNode != nullptr) {
                node->isAnonymous = storedTypeNode->isAnonymous;
            }
            return t;
        }

        std::any visitRangeType(ASTRangeType* node) override {
            DataType startType = std::any_cast<DataType>(node->startConstant->accept(this));
            DataType endType = std::any_cast<DataType>(node->endConstant->accept(this));

            // Subrange Tidak boleh berjenis Real
            if (startType == DataType::REAL || endType == DataType::REAL) {
                throw std::runtime_error("Semantic Error: Subrange tidak boleh memiliki tipe Real.");
            }

            // Subrange : Batas bawah dan atas harus bertipe sama
            if (startType != endType) {
                throw std::runtime_error("Semantic Error: Tipe data batas bawah dan batas atas subrange tidak cocok.");
            }

            // Subrange: Lower Bound tidak boleh lebih besar dari Upper Bound
            auto* startLit = dynamic_cast<ASTLiteralExpressionNode*>(node->startConstant);
            auto* endLit = dynamic_cast<ASTLiteralExpressionNode*>(node->endConstant);
            if (startLit && endLit) {
                if (startType == DataType::INTEGER) {
                    if (std::get<int>(startLit->value) > std::get<int>(endLit->value)) {
                        throw std::runtime_error("Semantic Error: Batas bawah subrange tidak boleh lebih besar dari batas atas.");
                    }
                } else if (startType == DataType::CHAR) {
                    if (std::get<char>(startLit->value) > std::get<char>(endLit->value)) {
                        throw std::runtime_error("Semantic Error: Batas bawah subrange tidak boleh lebih besar dari batas atas.");
                    }
                } else if (startType == DataType::BOOLEAN) {
                    if (std::get<bool>(startLit->value) > std::get<bool>(endLit->value)) {
                        throw std::runtime_error("Semantic Error: Batas bawah subrange tidak boleh lebih besar dari batas atas.");
                    }
                }
            }

            if (startLit && endLit) {
                if (startType == DataType::INTEGER) {
                    rangeBounds_[node] = {std::get<int>(startLit->value), std::get<int>(endLit->value)};
                } else if (startType == DataType::CHAR) {
                    rangeBounds_[node] = {std::get<char>(startLit->value), std::get<char>(endLit->value)};
                } else if (startType == DataType::BOOLEAN) {
                    rangeBounds_[node] = {std::get<bool>(startLit->value), std::get<bool>(endLit->value)};
                }
            }

            node->isAnonymous = true;
            node->evalType_ = DataType::RANGE; // based on constant type
            return DataType::RANGE;
        }

        std::any visitArrayTypeNode(ASTArrayTypeNode* node) override {
            DataType indexType = std::any_cast<DataType>(node->indexType->accept(this));
            
            // Array: Index type harus berupa simple type dan bukan Real
            if (!isOrdinalKind(indexType) || indexType == DataType::REAL) {
                throw std::runtime_error("Semantic Error: Index type dari Array tidak boleh bertipe Real.");
            }

            DataType elemType = std::any_cast<DataType>(node->elementType->accept(this));
            (void)elemType;
            node->isAnonymous = true;
            node->evalType_ = DataType::ARRAY;
            return DataType::ARRAY;
        }

        std::any visitRecordTypeNode(ASTRecordTypeNode* node) override {
            std::unordered_map<std::string, bool> seenFields;
            for (const auto& field : node->fields) {
                if (field.type != nullptr) {
                    field.type->accept(this);
                }
                for (const auto& identifier : field.identifiers) {
                    if (seenFields.find(identifier) != seenFields.end()) {
                        throw std::runtime_error("Semantic Error: Field record '" + identifier + "' duplikat.");
                    }
                    seenFields[identifier] = true;
                }
            }
            node->isAnonymous = true;
            node->evalType_ = DataType::RECORD;
            return DataType::RECORD;
        }

        std::any visitEnumeratedTypeNode(ASTEnumeratedTypeNode* node) override {
            std::unordered_map<std::string, bool> seen;
            for (const auto& element : node->elements) {
                if (seen.find(element) != seen.end()) {
                    throw std::runtime_error("Semantic Error: Identifier enum '" + element + "' duplikat.");
                }
                seen[element] = true;
            }
            node->isAnonymous = true;
            node->evalType_ = DataType::ENUMERATED;
            return DataType::ENUMERATED;
        }

        // subprogram
        std::any visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node) override {
            int existingIdx = symbolTable.lookup(node->name);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Prosedur '" + node->name + "' sudah dideklarasikan di scope ini!");
            }

            int procIdx = symbolTable.insertVariable(node->name, DataType::VOID);
            symbolTable.getIdentifier(procIdx).typeName = node->name;
            node->symbolRefIndex_ = procIdx;
            node->lexicalLevel_ = currentLevel;
            node->evalType_ = DataType::VOID;

            currentLevel++;
            symbolTable.enterBlock();

            for (const auto& paramGroup : node->parameters) {
                DataType paramType = std::any_cast<DataType>(paramGroup.type->accept(this));
                ASTTypeNode* paramTypeNode = resolveTypeNode(paramGroup.type);
                for (const auto& paramName : paramGroup.identifiers) {
                    int paramIdx = symbolTable.insertVariable(paramName, paramType);
                    symbolTable.getIdentifier(paramIdx).normal = true;
                    rememberIdentifierType(paramIdx, paramTypeNode != nullptr ? paramTypeNode : paramGroup.type);
                }
            }

            for (auto* localDecl : node->localDeclarations) {
                if (localDecl != nullptr) localDecl->accept(this);
            }

            if (node->body != nullptr) node->body->accept(this);

            symbolTable.exitBlock();
            currentLevel--;
            return {};
        }

        std::any visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node) override {
            DataType returnType = resolveTypeNameKind(node->returnTypeName);

            int existingIdx = symbolTable.lookup(node->name);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Fungsi '" + node->name + "' sudah dideklarasikan di scope ini!");
            }

            int funcIdx = symbolTable.insertVariable(node->name, returnType);
            symbolTable.getIdentifier(funcIdx).typeName = node->name;
            node->symbolRefIndex_ = funcIdx;
            node->lexicalLevel_ = currentLevel;
            node->evalType_ = returnType;

            currentLevel++;
            symbolTable.enterBlock();

            for (const auto& paramGroup : node->parameters) {
                DataType paramType = std::any_cast<DataType>(paramGroup.type->accept(this));
                ASTTypeNode* paramTypeNode = resolveTypeNode(paramGroup.type);
                for (const auto& paramName : paramGroup.identifiers) {
                    int paramIdx = symbolTable.insertVariable(paramName, paramType);
                    symbolTable.getIdentifier(paramIdx).normal = true;
                    rememberIdentifierType(paramIdx, paramTypeNode != nullptr ? paramTypeNode : paramGroup.type);
                }
            }

            for (auto* localDecl : node->localDeclarations) {
                if (localDecl != nullptr) localDecl->accept(this);
            }

            if (node->body != nullptr) node->body->accept(this);

            symbolTable.exitBlock();
            currentLevel--;
            return {};
        }

        std::any visitUnaryExpressionNode(ASTUnaryExpressionNode* node) override {
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

        // variabel / expression access
        std::any visitVariableExpressionNode(ASTVariableExpressionNode* node) override {
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

        std::any visitLiteralExpressionNode(ASTLiteralExpressionNode* node) override {
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

        std::any visitBinaryExpressionNode(ASTBinaryExpressionNode* node) override {
            DataType lhsType = std::any_cast<DataType>(node->lhs->accept(this));
            DataType rhsType = std::any_cast<DataType>(node->rhs->accept(this));
            ASTTypeNode* lhsTypeNode = dynamic_cast<ASTVariableExpressionNode*>(node->lhs) != nullptr ? getStoredTypeNode(node->lhs->symbolRefIndex_) : nullptr;
            ASTTypeNode* rhsTypeNode = dynamic_cast<ASTVariableExpressionNode*>(node->rhs) != nullptr ? getStoredTypeNode(node->rhs->symbolRefIndex_) : nullptr;
            DataType resultType = DataType::VOID;
            std::string op = node->op;

            // operator Relasional menghasilkan Boolean
            if (op == "==" || op == "<>" || op == "<" || op == ">" || op == "<=" || op == ">=") {
                if (!areTypesCompatible(lhsTypeNode, rhsTypeNode, lhsType, rhsType) &&
                    !(isNumericKind(lhsType) && isNumericKind(rhsType)) &&
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
            else if (op == "+" || op == "-" || op == "*") {
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

        std::any visitCallExpressionNode(ASTCallExpressionNode* node) override {
            int symbolIdx = symbolTable.lookup(node->callee);
            if (symbolIdx == -1) {
                throw std::runtime_error("Semantic Error: Fungsi/prosedur '" + node->callee + "' belum dideklarasikan.");
            }

            IdentifierTableEntry& entry = symbolTable.getIdentifier(symbolIdx);
            if (entry.type == DataType::VOID) {
                throw std::runtime_error("Semantic Error: Prosedur '" + node->callee + "' tidak dapat digunakan sebagai ekspresi.");
            }

            for (auto* argument : node->arguments) {
                if (argument != nullptr) {
                    argument->accept(this);
                }
            }

            node->symbolRefIndex_ = symbolIdx;
            node->lexicalLevel_ = entry.level;
            node->evalType_ = entry.type;
            return entry.type;
        }

        // statement validation
        std::any visitAssignmentStatementNode(ASTAssignmentStatementNode* node) override {
            DataType targetType = std::any_cast<DataType>(node->target->accept(this));
            DataType valueType = std::any_cast<DataType>(node->value->accept(this));
            ASTTypeNode* targetTypeNode = getStoredTypeNode(node->target->symbolRefIndex_);
            ASTTypeNode* valueTypeNode = dynamic_cast<ASTVariableExpressionNode*>(node->value) != nullptr ? getStoredTypeNode(node->value->symbolRefIndex_) : nullptr;

            validateAssignmentCompatibility(node->target, node->value, targetTypeNode, valueTypeNode, targetType, valueType);
            node->evalType_ = DataType::VOID;
            return {};
        }

        std::any visitIfStatementNode(ASTIfStatementNode* node) override {
            DataType condType = std::any_cast<DataType>(node->condition->accept(this));
            // Ekspresi di dalam IF-Statement HARUS bertipe Boolean
            if (condType != DataType::BOOLEAN) {
                throw std::runtime_error("Semantic Error: Kondisi di dalam IF-Statement harus menghasilkan tipe Boolean.");
            }
            if (node->thenBranch != nullptr) node->thenBranch->accept(this);
            if (node->elseBranch != nullptr) node->elseBranch->accept(this);
            node->evalType_ = DataType::VOID;
            return {};
        }

        std::any visitBlockStatementNode(ASTBlockStatementNode* node) override {
            for (auto* stmt : node->statements) {
                if (stmt != nullptr) stmt->accept(this);
            }
            node->evalType_ = DataType::VOID;
            return DataType::VOID;
        }

        std::any visitWhileStatementNode(ASTWhileStatementNode* node) override {
            DataType condType = std::any_cast<DataType>(node->condition->accept(this));
            if (condType != DataType::BOOLEAN) {
                throw std::runtime_error("Semantic Error: Kondisi di dalam WHILE-Statement harus menghasilkan tipe Boolean.");
            }
            if (node->body != nullptr) node->body->accept(this);
            node->evalType_ = DataType::VOID;
            return DataType::VOID;
        }

        std::any visitRepeatStatementNode(ASTRepeatStatementNode* node) override {
            for (auto* stmt : node->body) {
                if (stmt != nullptr) stmt->accept(this);
            }

            DataType condType = std::any_cast<DataType>(node->condition->accept(this));
            if (condType != DataType::BOOLEAN) {
                throw std::runtime_error("Semantic Error: Kondisi di dalam REPEAT-Statement harus menghasilkan tipe Boolean.");
            }
            node->evalType_ = DataType::VOID;
            return DataType::VOID;
        }

        std::any visitForStatementNode(ASTForStatementNode* node) override {
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

            if (node->body != nullptr) node->body->accept(this);
            node->symbolRefIndex_ = iteratorIdx;
            node->lexicalLevel_ = iteratorEntry.level;
            node->evalType_ = DataType::VOID;
            return DataType::VOID;
        }

        std::any visitCaseStatementNode(ASTCaseStatementNode* node) override {
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
                if (branch.body != nullptr) {
                    branch.body->accept(this);
                }
            }

            node->evalType_ = DataType::VOID;
            return DataType::VOID;
        }

        std::any visitCallStatementNode(ASTCallStatementNode* node) override {
            if (node->callExpr == nullptr) {
                throw std::runtime_error("Semantic Error: Call statement kosong.");
            }

            int symbolIdx = symbolTable.lookup(node->callExpr->callee);
            if (symbolIdx == -1) {
                throw std::runtime_error("Semantic Error: Fungsi/prosedur '" + node->callExpr->callee + "' belum dideklarasikan.");
            }

            IdentifierTableEntry& entry = symbolTable.getIdentifier(symbolIdx);
            for (auto* argument : node->callExpr->arguments) {
                if (argument != nullptr) {
                    argument->accept(this);
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
};