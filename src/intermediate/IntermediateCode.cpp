#include "IntermediateCode.hpp"

IntermediateCodeGenerator::IntermediateCodeGenerator(SymbolTable& symbolTable, SemanticAnalyzer& semanticAnalyzer)
    : symbolTable_(symbolTable), semanticAnalyzer_(semanticAnalyzer) {}

std::vector<Instruction> IntermediateCodeGenerator::generate(ASTProgramNode* root) {
    code_.clear();
    currentLexicalLevel_ = 0;
    currentFunctionNames_.clear();
    currentFunctionReturnSlots_.clear();
    pendingCallPatches_.clear();
    subprogramEntryLineBySymbolIndex_.clear();

    if (root == nullptr) throw std::runtime_error("Intermediate Error: AST root kosong.");

    root->accept(this);
    patchPendingCalls();
    return code_;
}

const std::vector<Instruction>& IntermediateCodeGenerator::getCode() const { return code_; }

std::string IntermediateCodeGenerator::dumpCode() const {
    std::ostringstream oss;
    for (int i = 0; i < static_cast<int>(code_.size()); ++i) {
        oss << code_[i].toString(i);
        if (i + 1 < static_cast<int>(code_.size())) {
            oss << '\n';
        }
    }
    return oss.str();
}

int IntermediateCodeGenerator::emit(OpCode op, int level, int operand) {
    code_.emplace_back(op, level, operand);
    return static_cast<int>(code_.size()) - 1;
}

int IntermediateCodeGenerator::emitLit(int value) {
    return emit(OpCode::LIT, 0, value);
}

int IntermediateCodeGenerator::emitLod(int level, int address) {
    return emit(OpCode::LOD, level, address);
}

int IntermediateCodeGenerator::emitSto(int level, int address) {
    return emit(OpCode::STO, level, address);
}

int IntermediateCodeGenerator::emitCal(int level, int targetLine) {
    return emit(OpCode::CAL, level, targetLine);
}

int IntermediateCodeGenerator::emitInt(int memorySize) {
    return emit(OpCode::INT, 0, memorySize);
}

int IntermediateCodeGenerator::emitJmp(int targetLine) {
    return emit(OpCode::JMP, 0, targetLine);
}

int IntermediateCodeGenerator::emitJpc(int targetLine) {
    return emit(OpCode::JPC, 0, targetLine);
}

int IntermediateCodeGenerator::emitOpr(OprCode opr) {
    return emit(OpCode::OPR, 0, static_cast<int>(opr));
}

int IntermediateCodeGenerator::emitRet() {
    return emit(OpCode::RET, 0, 0);
}

void IntermediateCodeGenerator::patchOperand(int instructionIndex, int newOperand) {
    if (instructionIndex < 0 || instructionIndex >= static_cast<int>(code_.size())) {
        throw std::runtime_error("Intermediate Error: indeks patch instruction tidak valid.");
    }
    code_[instructionIndex].setOperand(newOperand);
}

void IntermediateCodeGenerator::patchPendingCalls() {
    for (const PendingCallPatch& patch : pendingCallPatches_) {
        auto it = subprogramEntryLineBySymbolIndex_.find(patch.symbolIndex);
        if (it == subprogramEntryLineBySymbolIndex_.end()) {
            throw std::runtime_error(
                "Intermediate Error: alamat subprogram '" + patch.calleeName + "' belum diketahui."
            );
        }
        patchOperand(patch.instructionIndex, it->second);
    }
    pendingCallPatches_.clear();
}

int IntermediateCodeGenerator::currentLine() const {
    return static_cast<int>(code_.size());
}

int IntermediateCodeGenerator::getRuntimeAddress(ASTVariableExpressionNode* node) const {
    if (node == nullptr) {
        throw std::runtime_error("Intermediate Error: variable expression kosong.");
    }
    if (isFunctionReturnTarget(node)) {
        return currentFunctionReturnSlots_.back();
    }
    if (node->symbolRefIndex_ < 0) {
        throw std::runtime_error("Intermediate Error: variable '" + node->baseName + "' belum memiliki symbol reference.");
    }

    int runtimeAddress;

    // Cek apakah variabel ini punya offset lokal/parameter yang sudah di-set
    auto it = relativeOffsetMap_.find(node->symbolRefIndex_);
    if (it != relativeOffsetMap_.end()) {
        runtimeAddress = it->second; // Bisa negatif (parameter) atau positif (lokal)
    } 
    
    if (it != relativeOffsetMap_.end() && it->second < 0) {
        // Parameter yang berhasil dipetakan secara negatif
        runtimeAddress = it->second;
    } else if (it != relativeOffsetMap_.end() && it->second >= FRAME_HEADER_SIZE) {
         // Variabel lokal
        runtimeAddress = it->second;
    } else {
        // Fallback untuk Global Variable (Jika belum masuk map)
        const IdentifierTableEntry& entry = symbolTable_.getIdentifier(node->symbolRefIndex_);

        if (entry.level > 0 && entry.address < 10) { 
            //  Parameter paksa menjadi negatif
            runtimeAddress = -(entry.address + 1);
        } else {
            // Variabel global / lokal
            runtimeAddress = FRAME_HEADER_SIZE + entry.address; 
        }
    }

    if (!node->components.empty()) {
        ASTTypeNode* baseTypeNode = semanticAnalyzer_.getIdentifierTypeNode(node->symbolRefIndex_);
        runtimeAddress += computeStaticComponentOffset(node, baseTypeNode);
    }

    return runtimeAddress;
}

int IntermediateCodeGenerator::getRuntimeLevel(ASTVariableExpressionNode* node) const {
    if (isFunctionReturnTarget(node)) {
        return 0;
    }
    return getLevelDifference(node->lexicalLevel_);
}

int IntermediateCodeGenerator::getLevelDifference(int declarationLevel) const {
    if (currentLexicalLevel_ < declarationLevel) {
        return 0;
    }
    return currentLexicalLevel_ - declarationLevel;
}

int IntermediateCodeGenerator::computeProgramMemorySize(ASTProgramNode* node) const {
    if (node == nullptr) {
        return FRAME_HEADER_SIZE;
    }
    return FRAME_HEADER_SIZE + computeDeclarationsMemorySize(node->declarations);
}

int IntermediateCodeGenerator::computeDeclarationsMemorySize(ASTDeclarationsNode* node) const {
    if (node == nullptr) {
        return 0;
    }
    int maxEndAddress = 0;
    for (ASTDeclarationNode* declaration : node->declarations) {
        if (declaration == nullptr) {
            continue;
        }
        if (auto* varDecl = dynamic_cast<ASTVarDeclarationNode*>(declaration)) {
            maxEndAddress = std::max(maxEndAddress, computeVariableMemorySize(varDecl));
        } else if (auto* constDecl = dynamic_cast<ASTConstDeclarationNode*>(declaration)) {
            if (constDecl->symbolRefIndex_ >= 0) {
                const IdentifierTableEntry& entry = symbolTable_.getIdentifier(constDecl->symbolRefIndex_);
                maxEndAddress = std::max(maxEndAddress, entry.address + 1);
            }
        }
    }
    return maxEndAddress;
}

int IntermediateCodeGenerator::computeVariableMemorySize(ASTVarDeclarationNode* node) const {
    if (node == nullptr || node->symbolRefIndex_ < 0) {
        return 0;
    }
    const IdentifierTableEntry& entry = symbolTable_.getIdentifier(node->symbolRefIndex_);
    ASTTypeNode* typeNode = semanticAnalyzer_.getIdentifierTypeNode(node->symbolRefIndex_);
    int declaredSize = typeNode != nullptr ? semanticAnalyzer_.getTypeStorageSize(typeNode) : 1;
    return entry.address + std::max(1, declaredSize);
}

bool IntermediateCodeGenerator::isBuiltinProcedure(const std::string& name) const {
    return isWriteProcedure(name) || isWritelnProcedure(name);
}

bool IntermediateCodeGenerator::isWriteProcedure(const std::string& name) const {
    return name == "write";
}

bool IntermediateCodeGenerator::isWritelnProcedure(const std::string& name) const {
    return name == "writeln" || name == "println";
}

bool IntermediateCodeGenerator::isFunctionReturnTarget(ASTVariableExpressionNode* node) const {
    if (node == nullptr || !node->components.empty() || currentFunctionNames_.empty()) {
        return false;
    }
    return node->baseName == currentFunctionNames_.back();
}


OprCode IntermediateCodeGenerator::mapUnaryOperatorToOpr(const std::string& op) const {
    if (op == "-") {
        return OprCode::NEG;
    }
    throw std::runtime_error("Intermediate Error: operator unary '" + op + "' belum didukung langsung.");
}

OprCode IntermediateCodeGenerator::mapBinaryOperatorToOpr(const std::string& op) const {
    if (op == "+") return OprCode::ADD;
    if (op == "-") return OprCode::SUB;
    if (op == "*") return OprCode::MUL;
    if (op == "/") return OprCode::RDIV;
    if (op == "div") return OprCode::DIV;
    if (op == "mod") return OprCode::MOD;
    if (op == "==" || op == "=") return OprCode::EQL;
    if (op == "<>" || op == "!=") return OprCode::NEQ;
    if (op == "<") return OprCode::LSS;
    if (op == ">=") return OprCode::GEQ;
    if (op == ">") return OprCode::GTR;
    if (op == "<=") return OprCode::LEQ;

    throw std::runtime_error("Intermediate Error: operator binary '" + op + "' belum didukung.");
}

bool IntermediateCodeGenerator::constantExpressionToInt(ASTExpressionNode* expression, int& outValue) const {
    if (expression == nullptr) {
        return false;
    }

    if (auto* literal = dynamic_cast<ASTLiteralExpressionNode*>(expression)) {
        outValue = literalToInt(literal);
        return true;
    }

    if (auto* unary = dynamic_cast<ASTUnaryExpressionNode*>(expression)) {
        int operandValue = 0;
        if (!constantExpressionToInt(unary->operand, operandValue)) {
            return false;
        }
        if (unary->op == "-") {
            outValue = -operandValue;
            return true;
        }
        if (unary->op == "+") {
            outValue = operandValue;
            return true;
        }
    }

    if (auto* variable = dynamic_cast<ASTVariableExpressionNode*>(expression)) {
        int symbolIndex = symbolTable_.lookup(variable->baseName);
        if (symbolIndex != -1) {
            const IdentifierTableEntry& entry = symbolTable_.getIdentifier(symbolIndex);
            if (entry.isConstant && entry.obj == "constant" && variable->components.empty()) {
                outValue = entry.address;
                return true;
            }
        }
    }

    return false;
}

int IntermediateCodeGenerator::computeStaticComponentOffset(ASTVariableExpressionNode* node, ASTTypeNode* baseTypeNode) const {
    if (node == nullptr) {
        return 0;
    }

    int offset = 0;
    ASTTypeNode* currentTypeNode = semanticAnalyzer_.getResolvedTypeNode(baseTypeNode);

    for (const auto& component : node->components) {
        if (component.isArrayIndex) {
            for (ASTExpressionNode* indexExpression : component.indices) {
                auto* arrayType = dynamic_cast<ASTArrayTypeNode*>(semanticAnalyzer_.getResolvedTypeNode(currentTypeNode));
                if (arrayType == nullptr) {
                    throw std::runtime_error("Intermediate Error: akses indeks pada tipe non-array.");
                }

                int indexValue = 0;
                if (!constantExpressionToInt(indexExpression, indexValue)) {
                    throw std::runtime_error(
                        "Intermediate Error: indeks array dinamis belum dapat direpresentasikan dengan LOD/STO langsung. "
                        "Gunakan indeks literal untuk generator awal, atau tambahkan opcode indirect load/store pada interpreter."
                    );
                }

                int low = 0;
                int high = 0;
                ASTTypeNode* indexType = semanticAnalyzer_.getResolvedTypeNode(arrayType->indexType);
                semanticAnalyzer_.getRangeBounds(dynamic_cast<ASTRangeType*>(indexType), low, high);
                if (indexValue < low || indexValue > high) {
                    throw std::runtime_error("Intermediate Error: indeks array literal berada di luar range deklarasi.");
                }

                int elementSize = semanticAnalyzer_.getTypeStorageSize(arrayType->elementType);
                offset += (indexValue - low) * std::max(1, elementSize);
                currentTypeNode = semanticAnalyzer_.getResolvedTypeNode(arrayType->elementType);
            }
        } else {
            auto* recordType = dynamic_cast<ASTRecordTypeNode*>(semanticAnalyzer_.getResolvedTypeNode(currentTypeNode));
            ASTTypeNode* fieldType = nullptr;
            offset += computeRecordFieldOffset(recordType, component.fieldName, fieldType);
            currentTypeNode = semanticAnalyzer_.getResolvedTypeNode(fieldType);
        }
    }

    return offset;
}

int IntermediateCodeGenerator::computeRecordFieldOffset(ASTRecordTypeNode* recordTypeNode, const std::string& fieldName, ASTTypeNode*& fieldTypeNode) const {
    if (recordTypeNode == nullptr) {
        throw std::runtime_error("Intermediate Error: akses field pada tipe non-record.");
    }

    int offset = 0;
    for (const auto& field : recordTypeNode->fields) {
        int fieldSize = semanticAnalyzer_.getTypeStorageSize(field.type);
        for (const std::string& identifier : field.identifiers) {
            if (identifier == fieldName) {
                fieldTypeNode = semanticAnalyzer_.getResolvedTypeNode(field.type);
                return offset;
            }
            offset += std::max(1, fieldSize);
        }
    }

    throw std::runtime_error("Intermediate Error: field record '" + fieldName + "' tidak ditemukan.");
}

int IntermediateCodeGenerator::literalToInt(const ASTLiteralExpressionNode* node) const {
    if (node == nullptr) {
        throw std::runtime_error("Intermediate Error: literal kosong.");
    }

    if (std::holds_alternative<int>(node->value)) { 
        return std::get<int>(node->value);
    }
    if (std::holds_alternative<double>(node->value)) {
        return static_cast<int>(std::get<double>(node->value));
    }
    if (std::holds_alternative<char>(node->value)) {
        return static_cast<int>(std::get<char>(node->value));
    }
    if (std::holds_alternative<bool>(node->value)) {
        bool value = std::get<bool>(node->value);
        if (value) {
            return 1;
        } else {
            return 0;
        }
    }

    if (std::holds_alternative<std::string>(node->value)) {
        throw std::runtime_error("Intermediate Error: literal string belum didukung oleh instruksi LIT integer.");
    }
    throw std::runtime_error("Intermediate Error: tipe literal tidak dikenali.");
}

int IntermediateCodeGenerator::emitCallToSubprogram(ASTCallExpressionNode* callExpr) {
    if (callExpr == nullptr) {
        throw std::runtime_error("Intermediate Error: call expression kosong.");
    }

    for (ASTExpressionNode* arg : callExpr->arguments) {
        if (arg != nullptr) {
            arg->accept(this);
        }
    }

    int symbolIndex = callExpr->symbolRefIndex_ >= 0 ? callExpr->symbolRefIndex_ : symbolTable_.lookup(callExpr->callee);
    if (symbolIndex == -1) {
        throw std::runtime_error("Intermediate Error: prosedur/fungsi '" + callExpr->callee + "' tidak ditemukan.");
    }

    const IdentifierTableEntry& entry = symbolTable_.getIdentifier(symbolIndex);
    int levelDiff = getLevelDifference(entry.level);

    auto knownAddress = subprogramEntryLineBySymbolIndex_.find(symbolIndex);
    int targetLine = knownAddress != subprogramEntryLineBySymbolIndex_.end() ? knownAddress->second : 0;
    int callInstructionIndex = emitCal(levelDiff, targetLine);

    if (knownAddress == subprogramEntryLineBySymbolIndex_.end()) {
        pendingCallPatches_.push_back(PendingCallPatch{callInstructionIndex, symbolIndex, callExpr->callee});
    }

    return callInstructionIndex;
}
