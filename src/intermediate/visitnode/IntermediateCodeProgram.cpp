#include "../IntermediateCode.hpp"

std::any IntermediateCodeGenerator::visitProgramNode(ASTProgramNode* node) {
    if (node == nullptr) return {};
    
    currentLexicalLevel_ = 0;

    // Kalkulasi Variabel Global (Offset Positif)
    // Sisihkan 3 slot pertama (Static Link, Dynamic Link, return Address)
    int globalDataCells = 0;
    int currentGlobalOffset = FRAME_HEADER_SIZE; 
    
    if (node->declarations != nullptr) {
        for (auto* declaration : node->declarations->declarations) {
            if (declaration != nullptr) {
                if (auto* varDecl = dynamic_cast<ASTVarDeclarationNode*>(declaration)) {
                    if (varDecl->symbolRefIndex_ >= 0) {
                        // Masukkan ke Map Offset
                        relativeOffsetMap_[varDecl->symbolRefIndex_] = currentGlobalOffset;
                        
                        ASTTypeNode* typeNode = semanticAnalyzer_.getIdentifierTypeNode(varDecl->symbolRefIndex_);
                        int varSize = typeNode != nullptr ? semanticAnalyzer_.getTypeStorageSize(typeNode) : 1;
                        
                        currentGlobalOffset += std::max(1, varSize);
                        globalDataCells += std::max(1, varSize);
                    }
                }
                else if (auto* constDecl = dynamic_cast<ASTConstDeclarationNode*>(declaration)) {
                    if (constDecl->symbolRefIndex_ >= 0) {
                        relativeOffsetMap_[constDecl->symbolRefIndex_] = currentGlobalOffset;
                        currentGlobalOffset += 1;
                        globalDataCells += 1;
                    }
                }
            }
        }
    }

    // Instruksi INT khusus untuk ukuran program yang sebenarnya
    emitInt(FRAME_HEADER_SIZE + globalDataCells);

    // Kunjungi deklarasi konstanta, dll
    if (node->declarations != nullptr) {
        node->declarations->accept(this);
    }

    // Eksekusi blok utama (BEGIN ... END)
    if (node->mainBlock != nullptr) {
        node->mainBlock->accept(this);
    }

    emitRet();
    return {};
}