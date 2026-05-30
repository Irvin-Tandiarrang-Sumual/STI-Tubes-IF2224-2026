#include "../IntermediateCode.hpp"
#include "../Instruction.hpp"

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
    (void)node;
    // konstanta dievaluasi saat semantic analysis / compile-time
    // tidak menghasilkan runtime intermediate code
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

    int procIndex = symbolTable_.lookup(node->name);

    if (procIndex != -1) {
        IdentifierTableEntry& entry = symbolTable_.getIdentifier(procIndex);
        entry.address = procedureAddress;
    }

    int localMemorySize = FRAME_HEADER_SIZE;

    for (auto* declaration : node->localDeclarations) {
        if (auto* varDecl =
            dynamic_cast<ASTVarDeclarationNode*>(declaration)) {

            localMemorySize += computeVariableMemorySize(varDecl);
        }
    }

    emitInt(localMemorySize);

    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            declaration->accept(this);
        }
    }

    if (node->body != nullptr) {
        node->body->accept(this);
    }

    emitRet();

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

    int funcIndex = symbolTable_.lookup(node->name);

    if (funcIndex != -1) {
        IdentifierTableEntry& entry = symbolTable_.getIdentifier(funcIndex);
        entry.address = functionAddress;
    }

    int localMemorySize = FRAME_HEADER_SIZE;

    for (auto* declaration : node->localDeclarations) {
        if (auto* varDecl =
            dynamic_cast<ASTVarDeclarationNode*>(declaration)) {

            localMemorySize += computeVariableMemorySize(varDecl);
        }
    }

    localMemorySize += 1;

    emitInt(localMemorySize);

    for (auto* declaration : node->localDeclarations) {
        if (declaration != nullptr) {
            declaration->accept(this);
        }
    }

    if (node->body != nullptr) {
        node->body->accept(this);
    }

    emitRet();

    patchOperand(skipJumpIndex, currentLine());

    return {};
}