#include "IntermediateCode.hpp"

IntermediateCodeGenerator::IntermediateCodeGenerator(SymbolTable& symbolTable) : symbolTable_(symbolTable) {}

std::vector<Instruction> IntermediateCodeGenerator::generate(ASTProgramNode* root) {
    code_.clear();

    if (root == nullptr) throw std::runtime_error("Intermediate Error: AST root kosong.");

    root->accept(this);
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

int IntermediateCodeGenerator::currentLine() const {
    return static_cast<int>(code_.size());
}

int IntermediateCodeGenerator::getRuntimeAddress(ASTVariableExpressionNode* node) const {
    if (node == nullptr) {
        throw std::runtime_error("Intermediate Error: variable expression kosong.");
    }
    if (!node->components.empty()) {
        throw std::runtime_error("Intermediate Error: akses array/record belum didukung pada generator awal.");
    }
    if (node->symbolRefIndex_ < 0) {
        throw std::runtime_error("Intermediate Error: variable '" + node->baseName + "' belum memiliki symbol reference.");
    }
    const IdentifierTableEntry& entry = symbolTable_.getIdentifier(node->symbolRefIndex_);
    return FRAME_HEADER_SIZE + entry.address;
}

int IntermediateCodeGenerator::getLevelDifference(int declarationLevel) const {
    (void)declarationLevel;
    return 0;
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
    return entry.address + 1;
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
    if (op == "/" || op == "div") return OprCode::DIV;
    if (op == "mod") return OprCode::MOD;
    if (op == "==" || op == "=") return OprCode::EQL;
    if (op == "<>" || op == "!=") return OprCode::NEQ;
    if (op == "<") return OprCode::LSS;
    if (op == ">=") return OprCode::GEQ;
    if (op == ">") return OprCode::GTR;
    if (op == "<=") return OprCode::LEQ;

    throw std::runtime_error("Intermediate Error: operator binary '" + op + "' belum didukung.");
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