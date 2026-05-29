#include "Instruction.hpp"

#include <sstream>
#include <stdexcept>

Instruction::Instruction(OpCode op, int level, int operand)
    : op_(op), level_(level), operand_(operand) {}

OpCode Instruction::getOp() const {
    return op_;
}

int Instruction::getLevel() const {
    return level_;
}

int Instruction::getOperand() const {
    return operand_;
}

void Instruction::setOp(OpCode op) {
    op_ = op;
}

void Instruction::setLevel(int level) {
    level_ = level;
}

void Instruction::setOperand(int operand) {
    operand_ = operand;
}

std::string Instruction::toString(int lineNumber) const {
    std::ostringstream oss;
    oss << lineNumber << " " << opCodeToString(op_) << " " << level_ << " " << operand_;
    return oss.str();
}

std::string opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::LIT: return "LIT";
        case OpCode::LOD: return "LOD";
        case OpCode::STO: return "STO";
        case OpCode::CAL: return "CAL";
        case OpCode::INT: return "INT";
        case OpCode::JMP: return "JMP";
        case OpCode::JPC: return "JPC";
        case OpCode::OPR: return "OPR";
        case OpCode::RET: return "RET";
    }
    throw std::runtime_error("Intermediate Error: OpCode tidak dikenal.");
}

std::string oprCodeToString(OprCode op) {
    switch (op) {
        case OprCode::NEG: return "NEG";
        case OprCode::ADD: return "ADD";
        case OprCode::SUB: return "SUB";
        case OprCode::MUL: return "MUL";
        case OprCode::DIV: return "DIV";
        case OprCode::MOD: return "MOD";
        case OprCode::EQL: return "EQL";
        case OprCode::NEQ: return "NEQ";
        case OprCode::LSS: return "LSS";
        case OprCode::GEQ: return "GEQ";
        case OprCode::GTR: return "GTR";
        case OprCode::LEQ: return "LEQ";
        case OprCode::WRT: return "WRT";
        case OprCode::WRTLN: return "WRTLN";
    }
    throw std::runtime_error("Intermediate Error: OprCode tidak dikenal.");
}
