#pragma once

#include <string>

enum class OpCode {
    LIT,
    LOD,
    STO,
    CAL,
    INT,
    JMP,
    JPC,
    OPR,
    RET
};

enum class OprCode {
    NEG   = 1,
    ADD   = 2,
    SUB   = 3,
    MUL   = 4,
    DIV   = 5,
    MOD   = 6,

    EQL   = 7,
    NEQ   = 8,
    LSS   = 9,
    GEQ   = 10,
    GTR   = 11,
    LEQ   = 12,

    WRT   = 13,
    WRTLN = 14
};

class Instruction {
private:
    OpCode op_;
    int level_;
    int operand_;

public:
    Instruction(OpCode op, int level, int operand);

    OpCode getOp() const;
    int getLevel() const;
    int getOperand() const;

    void setOp(OpCode op);
    void setLevel(int level);
    void setOperand(int operand);

    std::string toString(int lineNumber) const;
};

std::string opCodeToString(OpCode op);
std::string oprCodeToString(OprCode op);