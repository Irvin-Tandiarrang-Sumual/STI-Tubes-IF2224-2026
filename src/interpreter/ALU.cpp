#include "ALU.hpp"
#include <stdexcept>

void ALU::execute(OprCode opr, RuntimeStack& memory) {
    // Operasi Unary
    if (opr == OprCode::NEG) {
        int val = memory.pop();
        memory.push(-val);
        return;
    }

    // Operasi Binary
    // Nilai operand kanan di-Pop terlebih dahulu
    int valRight = memory.pop();
    int valLeft = memory.pop();

    switch (opr) {
        case OprCode::ADD: memory.push(valLeft + valRight); break;
        case OprCode::SUB: memory.push(valLeft - valRight); break;
        case OprCode::MUL: memory.push(valLeft * valRight); break;
        case OprCode::DIV: 
            if (valRight == 0) throw std::runtime_error("Runtime Error: Division by zero.");
            memory.push(valLeft / valRight); 
            break;
        case OprCode::MOD: 
            if (valRight == 0) throw std::runtime_error("Runtime Error: Modulo by zero.");
            memory.push(valLeft % valRight); 
            break;
        // TODO: Operasi Perbandingan (EQL, NEQ, LSS, GEQ, GTR, LEQ) & Penulisan (WRT, WRTLN)
        default:
            break;
    }
}