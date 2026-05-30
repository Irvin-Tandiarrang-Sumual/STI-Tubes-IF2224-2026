#include <stdexcept>
#include <iostream>

#include "ALU.hpp"

void ALU::execute(OprCode opr, RuntimeStack& memory, std::ostream& outStream) {
    // Operasi Unary
    if (opr == OprCode::NEG) {
        int val = memory.pop();
        memory.push(-val);
        return;
    }

    // Operasi Output
    if (opr == OprCode::WRT) {
        int val = memory.pop();
        outStream << val;
        return;
    }
    
    if (opr == OprCode::WRTLN) {
        int val = memory.pop();
        outStream << val << std::endl;
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
        // TODO: Operasi Perbandingan (EQL, NEQ, LSS, GEQ, GTR, LEQ)
        default:
            break;
    }
}