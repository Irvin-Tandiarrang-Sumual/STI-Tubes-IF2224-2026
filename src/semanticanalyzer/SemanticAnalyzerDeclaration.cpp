#include "SemanticAnalyzer.hpp"

std::any SemanticAnalyzer::visitDeclarationsNode(ASTDeclarationsNode* node) {
    try {
        predeclareSubprograms(node->declarations);
    } catch (const std::exception& ex) {
        reportError(node, ex.what());
    }

    for (auto* decl : node->declarations) {
        safeVisitNode(decl);
    }

    predeclaredSubprogramNames_.erase(currentLevel);
    node->evalType_ = DataType::VOID;
    return {};
}

std::any SemanticAnalyzer::visitTypeDeclarationNode(ASTTypeDeclarationNode* node) {
    if (node->typeDefinition == nullptr) {
        throw std::runtime_error("Semantic Error: Definisi tipe '" + node->name + "' kosong.");
    }

    DataType typeKind = std::any_cast<DataType>(node->typeDefinition->accept(this));
    int typeReference = 0;

    if (auto* arrayType = dynamic_cast<ASTArrayTypeNode*>(resolveTypeNode(node->typeDefinition))) {
        typeReference = ensureArrayTypeEntry(arrayType);
    }

    int existingIdx = symbolTable.lookup(node->name);
    if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
        throw std::runtime_error("Semantic Error: Tipe '" + node->name + "' sudah dideklarasikan di scope ini!");
    }

    int refIdx = symbolTable.insertVariable(node->name, typeKind, typeReference);
    IdentifierTableEntry& entry = symbolTable.getIdentifier(refIdx);
    entry.typeName = node->name;
    entry.obj = "type";

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

std::any SemanticAnalyzer::visitConstDeclarationNode(ASTConstDeclarationNode* node) {
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
    entry.isConstant = true;
    entry.obj = "constant";

    node->symbolRefIndex_ = refIdx;
    node->lexicalLevel_ = currentLevel;
    node->evalType_ = constType;
    return constType;
}

std::any SemanticAnalyzer::visitVarDeclarationNode(ASTVarDeclarationNode* node) {
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

    int arrayRef = 0;
    if (varType == DataType::ARRAY) {
        if (auto* arrayType = dynamic_cast<ASTArrayTypeNode*>(resolvedTypeNode)) {
            arrayRef = ensureArrayTypeEntry(arrayType);
        }
    }

    int refIdx = symbolTable.insertVariable(varName, varType, arrayRef);
    
    symbolTable.getIdentifier(refIdx).typeName = registeredTypeName;
    symbolTable.getIdentifier(refIdx).obj = "variabel";
    rememberIdentifierType(refIdx, resolvedTypeNode != nullptr ? resolvedTypeNode : node->type);

    node->symbolRefIndex_ = refIdx;
    node->lexicalLevel_ = currentLevel;
    node->evalType_ = varType;

    return {};
}

std::any SemanticAnalyzer::visitPrimitiveType(ASTPrimitiveType* node) {
    DataType t = mapStringToDataType(node->type);
    node->evalType_ = t;
    node->isAnonymous = true;
    return t;
}

std::any SemanticAnalyzer::visitNamedTypeNode(ASTNamedTypeNode* node) {
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

std::any SemanticAnalyzer::visitRangeType(ASTRangeType* node) {
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

std::any SemanticAnalyzer::visitArrayTypeNode(ASTArrayTypeNode* node) {
    ensureArrayTypeEntry(node);
    node->isAnonymous = true;
    node->evalType_ = DataType::ARRAY;
    return DataType::ARRAY;
}

std::any SemanticAnalyzer::visitRecordTypeNode(ASTRecordTypeNode* node) {
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

std::any SemanticAnalyzer::visitEnumeratedTypeNode(ASTEnumeratedTypeNode* node) {
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

std::any SemanticAnalyzer::visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node) {
    int existingIdx = symbolTable.lookup(node->name);
    const bool isPredeclared = predeclaredSubprogramNames_[currentLevel].find(node->name) != predeclaredSubprogramNames_[currentLevel].end();
    if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel && !isPredeclared) {
        throw std::runtime_error("Semantic Error: Prosedur '" + node->name + "' sudah dideklarasikan di scope ini!");
    }

    int procIdx = existingIdx;
    if (procIdx == -1 || symbolTable.getIdentifier(procIdx).level != currentLevel) {
        procIdx = symbolTable.insertVariable(node->name, DataType::VOID);
        predeclaredSubprogramNames_[currentLevel].insert(node->name);
    }
    symbolTable.getIdentifier(procIdx).typeName = node->name;
    symbolTable.getIdentifier(procIdx).obj = "procedure";
    node->symbolRefIndex_ = procIdx;
    node->lexicalLevel_ = currentLevel;
    node->evalType_ = DataType::VOID;

    currentLevel++;
    symbolTable.enterBlock();
    int newBlockIdx = symbolTable.getCurrentBlockIdx(); // cari indeks block yg aktif
    symbolTable.getIdentifier(procIdx).reference = newBlockIdx; // set ref subprogram
    try {
        predeclareSubprograms(node->localDeclarations);
    } catch (const std::exception& ex) {
        reportError(node, ex.what());
    }

    for (const auto& paramGroup : node->parameters) {
        DataType paramType = std::any_cast<DataType>(paramGroup.type->accept(this));
        ASTTypeNode* paramTypeNode = resolveTypeNode(paramGroup.type);
        for (const auto& paramName : paramGroup.identifiers) {
            symbolTable.getIdentifier(procIdx).parameterTypes.push_back(paramType);
            int paramIdx = symbolTable.insertVariable(paramName, paramType);
            symbolTable.getIdentifier(paramIdx).normal = true;
            symbolTable.getIdentifier(paramIdx).obj = "parameter";
            rememberIdentifierType(paramIdx, paramTypeNode != nullptr ? paramTypeNode : paramGroup.type);
        }
    }

    for (auto* localDecl : node->localDeclarations) {
        safeVisitNode(localDecl);
    }

    safeVisitNode(node->body);

    predeclaredSubprogramNames_.erase(currentLevel);
    symbolTable.exitBlock();
    currentLevel--;
    return {};
}

std::any SemanticAnalyzer::visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node) {
    DataType returnType = resolveTypeNameKind(node->returnTypeName);

    int existingIdx = symbolTable.lookup(node->name);
    const bool isPredeclared = predeclaredSubprogramNames_[currentLevel].find(node->name) != predeclaredSubprogramNames_[currentLevel].end();
    if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel && !isPredeclared) {
        throw std::runtime_error("Semantic Error: Fungsi '" + node->name + "' sudah dideklarasikan di scope ini!");
    }

    int funcIdx = existingIdx;
    if (funcIdx == -1 || symbolTable.getIdentifier(funcIdx).level != currentLevel) {
        funcIdx = symbolTable.insertVariable(node->name, returnType);
        predeclaredSubprogramNames_[currentLevel].insert(node->name);
    }
    symbolTable.getIdentifier(funcIdx).typeName = node->name;
    symbolTable.getIdentifier(funcIdx).obj = "function";
    node->symbolRefIndex_ = funcIdx;
    node->lexicalLevel_ = currentLevel;
    node->evalType_ = returnType;

    currentLevel++;
    symbolTable.enterBlock();
    int newBlockIdx = symbolTable.getCurrentBlockIdx(); // cari indeks block yg aktif
    symbolTable.getIdentifier(funcIdx).reference = newBlockIdx; // set ref subprogram
    try {
        predeclareSubprograms(node->localDeclarations);
    } catch (const std::exception& ex) {
        reportError(node, ex.what());
    }

    for (const auto& paramGroup : node->parameters) {
        DataType paramType = std::any_cast<DataType>(paramGroup.type->accept(this));
        ASTTypeNode* paramTypeNode = resolveTypeNode(paramGroup.type);
        for (const auto& paramName : paramGroup.identifiers) {
            symbolTable.getIdentifier(funcIdx).parameterTypes.push_back(paramType);
            int paramIdx = symbolTable.insertVariable(paramName, paramType);
            symbolTable.getIdentifier(paramIdx).normal = true;
            symbolTable.getIdentifier(paramIdx).obj = "parameter";
            rememberIdentifierType(paramIdx, paramTypeNode != nullptr ? paramTypeNode : paramGroup.type);
        }
    }

    for (auto* localDecl : node->localDeclarations) {
        safeVisitNode(localDecl);
    }

    safeVisitNode(node->body);

    predeclaredSubprogramNames_.erase(currentLevel);
    symbolTable.exitBlock();
    currentLevel--;
    return {};
}

