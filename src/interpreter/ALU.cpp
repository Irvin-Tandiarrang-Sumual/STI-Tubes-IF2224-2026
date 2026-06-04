#include <stdexcept>
#include <iostream>
#include <string>

#include "ALU.hpp"

void ALU::execute(OprCode opr, RuntimeStack& memory, std::ostream& outStream) {
    // Operasi Unary
    if (opr == OprCode::NEG) {
        auto val = memory.pop();
        if (std::holds_alternative<int>(val)) {
            memory.push(-std::get<int>(val));
        } else {
            throw std::runtime_error("Runtime Error: Operator NEG tidak bisa digunakan pada string.");
        }
        return;
    }

    // Operasi Output
    if (opr == OprCode::WRT) {
        auto val = memory.pop();
        if (std::holds_alternative<int>(val)) {
            outStream << std::get<int>(val);
        } else {
            outStream << std::get<std::string>(val);
        }
        return;
    }
    
    if (opr == OprCode::WRTLN) {
        auto val = memory.pop();
        if (std::holds_alternative<int>(val)) {
            outStream << std::get<int>(val) << std::endl;
        } else {
            outStream << std::get<std::string>(val) << std::endl;
        }
        return;
    }

    // Operasi Binary
    // Nilai operand kanan di-Pop terlebih dahulu
    auto valRight = memory.pop();
    auto valLeft = memory.pop();

    bool leftIsInt = std::holds_alternative<int>(valLeft);
    bool rightIsInt = std::holds_alternative<int>(valRight);

    switch (opr) {
        case OprCode::ADD: {
            if (leftIsInt && rightIsInt) {
                long long res = static_cast<long long>(std::get<int>(valLeft)) + std::get<int>(valRight);
                if (res > 2147483647LL || res < -2147483648LL) throw NumericalOverflowException();
                memory.push(static_cast<int>(res));
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) + std::get<std::string>(valRight));
            } else {
                throw std::runtime_error("Runtime Error: Type mismatch pada operator ADD.");
            }
            break;
        }
        case OprCode::SUB: {
            if (leftIsInt && rightIsInt) {
                long long res = static_cast<long long>(std::get<int>(valLeft)) - std::get<int>(valRight);
                if (res > 2147483647LL || res < -2147483648LL) throw NumericalOverflowException();
                memory.push(static_cast<int>(res));
            } else {
                throw std::runtime_error("Runtime Error: Operator SUB hanya untuk integer.");
            }
            break;
        }
        case OprCode::MUL: {
            if (leftIsInt && rightIsInt) {
                long long res = static_cast<long long>(std::get<int>(valLeft)) * std::get<int>(valRight);
                if (res > 2147483647LL || res < -2147483648LL) throw NumericalOverflowException();
                memory.push(static_cast<int>(res));
            } else {
                throw std::runtime_error("Runtime Error: Operator MUL hanya untuk integer.");
            }
            break;
        }
        case OprCode::DIV: {
            if (leftIsInt && rightIsInt) {
                int rightVal = std::get<int>(valRight);
                if (rightVal == 0) throw std::runtime_error("Runtime Error: Division by zero.");
                memory.push(std::get<int>(valLeft) / rightVal); 
            } else {
                throw std::runtime_error("Runtime Error: Operator DIV hanya untuk integer.");
            }
            break;
        }
        case OprCode::MOD: {
            if (leftIsInt && rightIsInt) {
                int rightVal = std::get<int>(valRight);
                if (rightVal == 0) throw std::runtime_error("Runtime Error: Modulo by zero.");
                memory.push(std::get<int>(valLeft) % rightVal); 
            } else {
                throw std::runtime_error("Runtime Error: Operator MOD hanya untuk integer.");
            }
            break;
        }
        case OprCode::EQL: {
            if (leftIsInt && rightIsInt) {
                memory.push(std::get<int>(valLeft) == std::get<int>(valRight) ? 1 : 0);
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) == std::get<std::string>(valRight) ? 1 : 0);
            } else {
                memory.push(0); // Beda tipe dianggap tidak sama
            }
            break;
        }
        case OprCode::NEQ: {
            if (leftIsInt && rightIsInt) {
                memory.push(std::get<int>(valLeft) != std::get<int>(valRight) ? 1 : 0);
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) != std::get<std::string>(valRight) ? 1 : 0);
            } else {
                memory.push(1); 
            }
            break;
        }
        case OprCode::LSS: {
            if (leftIsInt && rightIsInt) {
                memory.push(std::get<int>(valLeft) < std::get<int>(valRight) ? 1 : 0);
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) < std::get<std::string>(valRight) ? 1 : 0);
            } else {
                throw std::runtime_error("Runtime Error: Type mismatch pada operator perbandingan.");
            }
            break;
        }
        case OprCode::GEQ: {
            if (leftIsInt && rightIsInt) {
                memory.push(std::get<int>(valLeft) >= std::get<int>(valRight) ? 1 : 0);
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) >= std::get<std::string>(valRight) ? 1 : 0);
            } else {
                throw std::runtime_error("Runtime Error: Type mismatch pada operator perbandingan.");
            }
            break;
        }
        case OprCode::GTR: {
            if (leftIsInt && rightIsInt) {
                memory.push(std::get<int>(valLeft) > std::get<int>(valRight) ? 1 : 0);
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) > std::get<std::string>(valRight) ? 1 : 0);
            } else {
                throw std::runtime_error("Runtime Error: Type mismatch pada operator perbandingan.");
            }
            break;
        }
        case OprCode::LEQ: {
             if (leftIsInt && rightIsInt) {
                memory.push(std::get<int>(valLeft) <= std::get<int>(valRight) ? 1 : 0);
            } else if (!leftIsInt && !rightIsInt) {
                memory.push(std::get<std::string>(valLeft) <= std::get<std::string>(valRight) ? 1 : 0);
            } else {
                throw std::runtime_error("Runtime Error: Type mismatch pada operator perbandingan.");
            }
            break;
        }
        default:
            break;
    }
}