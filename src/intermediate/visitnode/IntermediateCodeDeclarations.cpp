#include "../IntermediateCode.hpp"
#include "../Instruction.hpp"
#include <algorithm>

std::any IntermediateCodeGenerator::visitDeclarationsNode(ASTDeclarationsNode* node) {
    if (node == nullptr) return {};
    for (auto* declaration : node->declarations) {
        if (declaration != nullptr) declaration->accept(this);
    }
    return {};
}

std::any IntermediateCodeGenerator::visitDeclarationNode(ASTDeclarationNode* node) {
    if (node == nullptr) return {};
    return node->accept(this);
}

std::any IntermediateCodeGenerator::visitConstDeclarationNode(ASTConstDeclarationNode* node) {
    if (node == nullptr || node->value == nullptr) return {};
    
    // Evaluasi nilai konstantanya
    node->value->accept(this);
    
    if (node->symbolRefIndex_ >= 0) {
        const IdentifierTableEntry& entry = symbolTable_.getIdentifier(node->symbolRefIndex_);
        
        // Cari alamatnya di Map Offset Lokal
        int targetAddress = FRAME_HEADER_SIZE + entry.address;
        
        auto it = relativeOffsetMap_.find(node->symbolRefIndex_);
        if (it != relativeOffsetMap_.end()) {
            targetAddress = it->second; // Ganti ke alamat offset yang benar
        }
        
        emitSto(getLevelDifference(entry.level), targetAddress);
    }
    return {};
}

std::any IntermediateCodeGenerator::visitTypeDeclarationNode(ASTTypeDeclarationNode* node) {
    (void)node; 
    return {};
}

std::any IntermediateCodeGenerator::visitVarDeclarationNode(ASTVarDeclarationNode* node) {
    (void)node; 
    return {};
}

std::any IntermediateCodeGenerator::visitSubprogramDeclarationNode(ASTSubprogramDeclarationNode* node) {
    if (node == nullptr) return {};
    return node->accept(this);
}

std::any IntermediateCodeGenerator::visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node) {
    if (node == nullptr) return {};

    int skipJumpIndex = emitJmp(0);
    int procedureAddress = currentLine();

    int procIndex = node->symbolRefIndex_ >= 0 ? node->symbolRefIndex_ : symbolTable_.lookup(node->name);
    if (procIndex != -1) {
        subprogramEntryLineBySymbolIndex_[procIndex] = procedureAddress;
    }

    int savedLevel = currentLexicalLevel_;
    currentLexicalLevel_ = node->lexicalLevel_ + 1;

    // Kalkulasi Parameter (Offset Negatif)
    int totalParams = 0;
    for (const auto& group : node->parameters) {
        totalParams += group.identifiers.size();
    }
    
    int currentParamOffset = -totalParams;
    for (const auto& group : node->parameters) {
        for (const std::string& paramName : group.identifiers) {
            
            // Searching tembus batasan scope
            int paramIndex = -1;
            for (int i = 0; i < 10000; ++i) { 
                try {
                    const IdentifierTableEntry& e = symbolTable_.getIdentifier(i);
                    if (e.name == paramName && e.level == currentLexicalLevel_) {
                        paramIndex = i;
                        break;
                    }
                } catch (...) {
                    break; 
                }
            }

            if (paramIndex >= 0) {
                relativeOffsetMap_[paramIndex] = currentParamOffset;
            }
            currentParamOffset += 1;
        }
    }

    // Kalkulasi Variabel Lokal (Offset positif)
    int localDataCells = 0;
    int currentLocalOffset = FRAME_HEADER_SIZE; 
    
    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            if (auto* varDecl = dynamic_cast<ASTVarDeclarationNode*>(declaration)) {
                if (varDecl->symbolRefIndex_ >= 0) {
                    relativeOffsetMap_[varDecl->symbolRefIndex_] = currentLocalOffset;
                    ASTTypeNode* typeNode = semanticAnalyzer_.getIdentifierTypeNode(varDecl->symbolRefIndex_);
                    int varSize = typeNode != nullptr ? semanticAnalyzer_.getTypeStorageSize(typeNode) : 1;
                    currentLocalOffset += std::max(1, varSize);
                    localDataCells += std::max(1, varSize);
                }
            }
        }
    }

    emitInt(FRAME_HEADER_SIZE + localDataCells);

    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) declaration->accept(this);
    }
    if (node->body != nullptr) node->body->accept(this);

    emitRet(totalParams, false);
    currentLexicalLevel_ = savedLevel;
    patchOperand(skipJumpIndex, currentLine());

    return {};
}

std::any IntermediateCodeGenerator::visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node) {
    if (node == nullptr) return {};

    int skipJumpIndex = emitJmp(0);
    int functionAddress = currentLine();

    int funcIndex = node->symbolRefIndex_ >= 0 ? node->symbolRefIndex_ : symbolTable_.lookup(node->name);
    if (funcIndex != -1) {
        subprogramEntryLineBySymbolIndex_[funcIndex] = functionAddress;
    }

    int savedLevel = currentLexicalLevel_;
    currentLexicalLevel_ = node->lexicalLevel_ + 1;

    // Kalkulasi Parameter (Offset Negatif)
    int totalParams = 0;
    for (const auto& group : node->parameters) {
        totalParams += group.identifiers.size();
    }
    
    int currentParamOffset = -totalParams; 
    for (const auto& group : node->parameters) {
        for (const std::string& paramName : group.identifiers) {
            
            int paramIndex = -1;
            for (int i = 0; i < 10000; ++i) {
                try {
                    const IdentifierTableEntry& e = symbolTable_.getIdentifier(i);
                    if (e.name == paramName && e.level == currentLexicalLevel_) {
                        paramIndex = i;
                        break;
                    }
                } catch (...) {
                    break;
                }
            }

            if (paramIndex >= 0) {
                relativeOffsetMap_[paramIndex] = currentParamOffset;
            }
            currentParamOffset += 1; 
        }
    }

    // Kalkulasi Variabel Lokal (Offset positif)
    int localDataCells = 0;
    int currentLocalOffset = FRAME_HEADER_SIZE; 
    
    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            if (auto* varDecl = dynamic_cast<ASTVarDeclarationNode*>(declaration)) {
                if (varDecl->symbolRefIndex_ >= 0) {
                    relativeOffsetMap_[varDecl->symbolRefIndex_] = currentLocalOffset;
                    ASTTypeNode* typeNode = semanticAnalyzer_.getIdentifierTypeNode(varDecl->symbolRefIndex_);
                    int varSize = typeNode != nullptr ? semanticAnalyzer_.getTypeStorageSize(typeNode) : 1;
                    currentLocalOffset += std::max(1, varSize);
                    localDataCells += std::max(1, varSize);
                }
            }
        }
    }

    int returnSlotAddress = currentLocalOffset; 
    emitInt(returnSlotAddress + 1);

    currentFunctionNames_.push_back(node->name);
    currentFunctionReturnSlots_.push_back(returnSlotAddress);

    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) declaration->accept(this);
    }
    if (node->body != nullptr) node->body->accept(this);

    emitLod(0, returnSlotAddress);
    emitRet(totalParams, true);

    currentFunctionReturnSlots_.pop_back();
    currentFunctionNames_.pop_back();
    currentLexicalLevel_ = savedLevel;

    patchOperand(skipJumpIndex, currentLine());
    return {};
}