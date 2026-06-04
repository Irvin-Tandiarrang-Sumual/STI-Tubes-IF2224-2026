#include "../IntermediateCode.hpp"
#include "../Instruction.hpp"

std::any IntermediateCodeGenerator::visitStatementNode(ASTStatementNode* node) {
    if (node != nullptr) {
        node->accept(this);
    }
    return std::any();
}

std::any IntermediateCodeGenerator::visitEmptyStatementNode(ASTEmptyStatementNode* node) {
    (void)node;
    return std::any();
}

std::any IntermediateCodeGenerator::visitBlockStatementNode(ASTBlockStatementNode* node) {
    if (node == nullptr) return std::any();
    
    for (ASTStatementNode* stmt : node->statements) {
        if (stmt != nullptr) {
            stmt->accept(this);
        }
    }
    return std::any();
}

std::any IntermediateCodeGenerator::visitAssignmentStatementNode(ASTAssignmentStatementNode* node) {
    if (node == nullptr) return std::any();

    // evaluasi sisi kanannya
    if (node->value != nullptr) {
        node->value->accept(this);
    }

    // cari address ruas kiri
    int address = getRuntimeAddress(node->target);
    int levelDiff = getRuntimeLevel(node->target);

    emitSto(levelDiff, address);
    return std::any();
}

std::any IntermediateCodeGenerator::visitIfStatementNode(ASTIfStatementNode* node) {
    if (node == nullptr) return std::any();

    if (node->condition != nullptr) {
        node->condition->accept(this);
    }

    // dummy address
    int jpcIdx = emitJpc(0);

    // block then
    if (node->thenBranch != nullptr) {
        node->thenBranch->accept(this);
    }

    if (node->elseBranch != nullptr) {
        int jmpIdx = emitJmp(0);

        // awal dari else
        patchOperand(jpcIdx, currentLine());

        // eval else
        node->elseBranch->accept(this);

        // setelah else
        patchOperand(jmpIdx, currentLine());
    } else {
        // backpatch-nya
        patchOperand(jpcIdx, currentLine());
    }

    return std::any();
}

std::any IntermediateCodeGenerator::visitWhileStatementNode(ASTWhileStatementNode* node) {
    if (node == nullptr) return std::any();

    int conditionLine = currentLine();

    if (node->condition != nullptr) {
        node->condition->accept(this);
    }

    // keluar dari loop kalo condition ga terpenuhi
    int jpcIdx = emitJpc(0);

    // jalanin di dalam
    if (node->body != nullptr) {
        node->body->accept(this);
    }

    // cek ulg kondisinya
    emitJmp(conditionLine);

    // backpatch keluar loop JPC ke baris setelah instruksi JMP
    patchOperand(jpcIdx, currentLine());

    return std::any();
}

std::any IntermediateCodeGenerator::visitRepeatStatementNode(ASTRepeatStatementNode* node) {
    if (node == nullptr) return std::any();

    int loopStartLine = currentLine();

    // jalanin statement di dalamnya
    for (ASTStatementNode* stmt : node->body) {
        if (stmt != nullptr) {
            stmt->accept(this);
        }
    }

    if (node->condition != nullptr) {
        node->condition->accept(this);
    }

    // Jika ekspresi kondisi bernilai false, JPC akan balik ke loopStartLine.
    emitJpc(loopStartLine);

    return std::any();
}

std::any IntermediateCodeGenerator::visitForStatementNode(ASTForStatementNode* node) {
    if (node == nullptr) return std::any();

    ASTVariableExpressionNode* iteratorVariable = nullptr;
    if (!node->iteratorName.empty()) {
        int iteratorIndex = symbolTable_.lookup(node->iteratorName);
        if (iteratorIndex == -1) {
            throw std::runtime_error("Intermediate Error: iterator '" + node->iteratorName + "' tidak ditemukan.");
        }

        auto& iteratorEntry = symbolTable_.getIdentifier(iteratorIndex);
        iteratorVariable = new ASTVariableExpressionNode(node->iteratorName, {}, node->location_);
        iteratorVariable->symbolRefIndex_ = iteratorIndex;
        iteratorVariable->lexicalLevel_ = iteratorEntry.level;
    }

    if (iteratorVariable == nullptr) {
        throw std::runtime_error("Intermediate Error: iterator FOR kosong.");
    }

    int varAddress = getRuntimeAddress(iteratorVariable);
    int levelDiff = getLevelDifference(iteratorVariable->lexicalLevel_);

    // batas awal
    if (node->startVal != nullptr) {
        node->startVal->accept(this);
    }
    emitSto(levelDiff, varAddress);

    // mark pengecekan kondisi batas atas/bawah loop
    int checkConditionLine = currentLine();

    // load variable value iterator saat ini sm batas akhirnya ke stack
    emitLod(levelDiff, varAddress);
    if (node->endVal != nullptr) {
        node->endVal->accept(this);
    }

    // milih arah loop up/down
    if (!node->isDownTo) {
        emitOpr(OprCode::LEQ);// <= limit
    } else {
        emitOpr(OprCode::GEQ);
    }

    // kondisi ga memenuhi
    int jpcIdx = emitJpc(0);

    // isi block
    if (node->body != nullptr) {
        node->body->accept(this);
    }

    // iteratornya naikin / turunin
    emitLod(levelDiff, varAddress);
    emitLit(1);
    if (!node->isDownTo) {
        emitOpr(OprCode::ADD); // iterator + 1
    } else {
        emitOpr(OprCode::SUB); // iterator - 1
    }
    emitSto(levelDiff, varAddress);

    // balik ke titik pengecekan kondisi batas loop
    emitJmp(checkConditionLine);

    // backpatch keluar JPC ke baris instruksi setelah loop berakhir
    patchOperand(jpcIdx, currentLine());

    return std::any();
}

std::any IntermediateCodeGenerator::visitCaseStatementNode(ASTCaseStatementNode* node) {
    if (node == nullptr) return std::any();

    std::vector<int> endJumps; // alamat instruksi break exit tiap cabang

    // uji ke setiap cabang
    for (ASTCaseBranchNode& branch : node->branches) {
        if (branch.constants.empty()) {
            if (branch.body != nullptr) {
                branch.body->accept(this);
            }
            endJumps.push_back(emitJmp(0));
            continue;
        }

        // setiap konstanta di dalam satu branch diuji satu per satu.
        // kalo cocok, eksekusi bodnya trs lgsg lompat keluar CASE.
        for (ASTExpressionNode* constant : branch.constants) {
            if (node->condition != nullptr) {
                node->condition->accept(this);
            }

            if (constant != nullptr) {
                constant->accept(this);
            }
            emitOpr(OprCode::EQL);

            int jpcToNextCheck = emitJpc(0);

            if (branch.body != nullptr) {
                branch.body->accept(this);
            }

            endJumps.push_back(emitJmp(0));
            patchOperand(jpcToNextCheck, currentLine());
        }
    }

    // backpatch semua penanda instruksi keluar (break-jumps) ke titik akhir struktur blok CASE
    for (int exitJmpIdx : endJumps) {
        patchOperand(exitJmpIdx, currentLine());
    }

    return std::any();
}

std::any IntermediateCodeGenerator::visitCallStatementNode(ASTCallStatementNode* node) {
    if (node == nullptr) return std::any();

    if (node->callExpr == nullptr) {
        throw std::runtime_error("Intermediate Error: call statement kosong.");
    }

    std::string procName = node->callExpr->callee;

    // built-in procedure etc
    if (isBuiltinProcedure(procName)) {
        for (size_t i = 0; i < node->callExpr->arguments.size(); ++i) {
            ASTExpressionNode* arg = node->callExpr->arguments[i];

            if (arg != nullptr) {
                arg->accept(this);

                if (isWritelnProcedure(procName) && i + 1 == node->callExpr->arguments.size()) {
                    emitOpr(OprCode::WRTLN);
                } else {
                    emitOpr(OprCode::WRT);
                }
            }
        }

        if (isWritelnProcedure(procName) && node->callExpr->arguments.empty()) {
            emitOpr(OprCode::WRTLN);
        }

        return {};
    }
    // defined by user
    else {
        emitCallToSubprogram(node->callExpr);
    }

    return std::any();
}