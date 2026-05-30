#include "../IntermediateCode.hpp"
#include "../Instruction.hpp"

#include <algorithm>

std::any IntermediateCodeGenerator::visitDeclarationsNode(ASTDeclarationsNode* node) {
    if (node == nullptr) {
        return {};
    }

    for (auto* declaration : node->declarations) {
        if (declaration != nullptr) {
            declaration->accept(this);
        }
    }

    return {};
}

std::any IntermediateCodeGenerator::visitDeclarationNode(ASTDeclarationNode* node) {
    if (node == nullptr) {
        return {};
    }

    return node->accept(this);
}

std::any IntermediateCodeGenerator::visitConstDeclarationNode(ASTConstDeclarationNode* node) {
    if (node == nullptr || node->value == nullptr) {
        return {};
    }
    node->value->accept(this);
    if (node->symbolRefIndex_ >= 0) {
        const IdentifierTableEntry& entry = symbolTable_.getIdentifier(node->symbolRefIndex_);
        emitSto(getLevelDifference(entry.level), FRAME_HEADER_SIZE + entry.address);
    }
    return {};
}

std::any IntermediateCodeGenerator::visitTypeDeclarationNode(ASTTypeDeclarationNode* node) {
    (void)node;
    // type declaration hanya metadata compile-time
    // tidak menghasilkan runtime code
    return {};
}

std::any IntermediateCodeGenerator::visitVarDeclarationNode(ASTVarDeclarationNode* node) {
    (void)node;
    // alokasi memory variable dilakukan lewat INT
    // pada scope block/procedure terkait
    return {};
}

std::any IntermediateCodeGenerator::visitSubprogramDeclarationNode(ASTSubprogramDeclarationNode* node) {
    if (node == nullptr) {
        return {};
    }

    return node->accept(this);
}

std::any IntermediateCodeGenerator::visitProcedureDeclarationNode(
    ASTProcedureDeclarationNode* node
) {
    if (node == nullptr) {
        return {};
    }

    int skipJumpIndex = emitJmp(0);

    int procedureAddress = currentLine();

    int procIndex;
    if(node->symbolRefIndex_ >= 0){
        procIndex = node->symbolRefIndex_;
    } else {
        procIndex = symbolTable_.lookup(node->name);
    }
    if (procIndex != -1) {
        subprogramEntryLineBySymbolIndex_[procIndex] = procedureAddress;
    }

    int savedLevel = currentLexicalLevel_;
    currentLexicalLevel_ = node->lexicalLevel_ + 1;

    int parameterCells = 0;
    for (const auto& group : node->parameters) {
        for (const std::string& paramName : group.identifiers) {
            int paramIndex = symbolTable_.lookup(paramName);
            if (paramIndex >= 0) {
                const IdentifierTableEntry& paramEntry = symbolTable_.getIdentifier(paramIndex);
                int paramSize = group.type != nullptr ? semanticAnalyzer_.getTypeStorageSize(group.type) : 1;
                parameterCells = std::max(parameterCells, paramEntry.address + std::max(1, paramSize));
            } else {
                parameterCells += 1;
            }
        }
    }

    int localDataCells = 0;
    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            if (auto* varDecl = dynamic_cast<ASTVarDeclarationNode*>(declaration)) {
                localDataCells = std::max(localDataCells, computeVariableMemorySize(varDecl));
            } else if (auto* constDecl = dynamic_cast<ASTConstDeclarationNode*>(declaration)) {
                if (constDecl->symbolRefIndex_ >= 0) {
                    const IdentifierTableEntry& entry = symbolTable_.getIdentifier(constDecl->symbolRefIndex_);
                    localDataCells = std::max(localDataCells, entry.address + 1);
                }
            }
        }
    }

    emitInt(FRAME_HEADER_SIZE + std::max(parameterCells, localDataCells));

    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            declaration->accept(this);
        }
    }

    if (node->body != nullptr) {
        node->body->accept(this);
    }

    emitRet();

    currentLexicalLevel_ = savedLevel;
    patchOperand(skipJumpIndex, currentLine());

    return {};
}
std::any IntermediateCodeGenerator::visitFunctionDeclarationNode(
    ASTFunctionDeclarationNode* node
) {
    if (node == nullptr) {
        return {};
    }

    int skipJumpIndex = emitJmp(0);
    int functionAddress = currentLine();

    int funcIndex;
    if(node->symbolRefIndex_ >= 0) {
        funcIndex = node->symbolRefIndex_;
    } else {
        funcIndex = symbolTable_.lookup(node->name);
    }
    if (funcIndex != -1) {
        subprogramEntryLineBySymbolIndex_[funcIndex] = functionAddress;
    }

    int savedLevel = currentLexicalLevel_;
    currentLexicalLevel_ = node->lexicalLevel_ + 1;

    int parameterCells = 0;
    for (const auto& group : node->parameters) {
        for (const std::string& paramName : group.identifiers) {
            int paramIndex = symbolTable_.lookup(paramName);
            if (paramIndex >= 0) {
                const IdentifierTableEntry& paramEntry = symbolTable_.getIdentifier(paramIndex);
                int paramSize = group.type != nullptr ? semanticAnalyzer_.getTypeStorageSize(group.type) : 1;
                parameterCells = std::max(parameterCells, paramEntry.address + std::max(1, paramSize));
            } else {
                parameterCells += 1;
            }
        }
    }

    int localDataCells = 0;
    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            if (auto* varDecl = dynamic_cast<ASTVarDeclarationNode*>(declaration)) {
                localDataCells = std::max(localDataCells, computeVariableMemorySize(varDecl));
            } else if (auto* constDecl = dynamic_cast<ASTConstDeclarationNode*>(declaration)) {
                if (constDecl->symbolRefIndex_ >= 0) {
                    const IdentifierTableEntry& entry = symbolTable_.getIdentifier(constDecl->symbolRefIndex_);
                    localDataCells = std::max(localDataCells, entry.address + 1);
                }
            }
        }
    }

    int dataCells = std::max(parameterCells, localDataCells);
    int returnSlotAddress = FRAME_HEADER_SIZE + dataCells;

    emitInt(returnSlotAddress + 1);

    currentFunctionNames_.push_back(node->name);
    currentFunctionReturnSlots_.push_back(returnSlotAddress);

    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            declaration->accept(this);
        }
    }

    if (node->body != nullptr) {
        node->body->accept(this);
    }

    // Konvensi generator: sebelum RET, function value dimuat ke stack.
    // Interpreter nantinya cukup mempertahankan nilai teratas sebagai hasil CAL.
    emitLod(0, returnSlotAddress);
    emitRet();

    currentFunctionReturnSlots_.pop_back();
    currentFunctionNames_.pop_back();
    currentLexicalLevel_ = savedLevel;

    patchOperand(skipJumpIndex, currentLine());
    return {};
}