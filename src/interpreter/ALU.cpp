#include <stdexcept>
#include <iostream>
#include <string>

#include "ALU.hpp"

// Helper Functions
static double getAsDouble(const std::variant<int, double, char, std::string>& v) {
    if (std::holds_alternative<int>(v)) return std::get<int>(v);
    if (std::holds_alternative<double>(v)) return std::get<double>(v);
    if (std::holds_alternative<char>(v)) return std::get<char>(v);
    return 0.0;
}

static bool isString(const std::variant<int, double, char, std::string>& v) {
    return std::holds_alternative<std::string>(v);
}

static bool isDouble(const std::variant<int, double, char, std::string>& v) {
    return std::holds_alternative<double>(v);
}

void ALU::execute(OprCode opr, RuntimeStack& memory, std::ostream& outStream) {
    // Operasi Unary
    if (opr == OprCode::NEG) {
        auto val = memory.pop();
        if (std::holds_alternative<int>(val)) {
            memory.push(-std::get<int>(val));
        } else if (std::holds_alternative<double>(val)) {
            memory.push(-std::get<double>(val));
        } else {
            throw std::runtime_error("Runtime Error: Operator NEG hanya untuk tipe numerik.");
        }
        return;
    }

    // Operasi Output
    if (opr == OprCode::WRT || opr == OprCode::WRTLN) {
        auto val = memory.pop();
        std::visit([&outStream](auto&& arg) {
            outStream << arg;
        }, val);
        if (opr == OprCode::WRTLN) outStream << std::endl;
        return;
    }

    // Operasi Binary
    // Nilai operand kanan di-Pop terlebih dahulu
    auto valRight = memory.pop();
    auto valLeft = memory.pop();

    bool hasString = isString(valLeft) || isString(valRight);
    bool hasDouble = isDouble(valLeft) || isDouble(valRight);

    switch (opr) {
        case OprCode::ADD: {
            if (hasString) {
                std::string lStr, rStr;
                if (isString(valLeft)) lStr = std::get<std::string>(valLeft);
                else if (std::holds_alternative<char>(valLeft)) lStr = std::string(1, std::get<char>(valLeft));
                
                if (isString(valRight)) rStr = std::get<std::string>(valRight);
                else if (std::holds_alternative<char>(valRight)) rStr = std::string(1, std::get<char>(valRight));
                
                memory.push(lStr + rStr);
            } else if (hasDouble) {
                memory.push(getAsDouble(valLeft) + getAsDouble(valRight)); // Hasil Double
            } else {
                long long res = static_cast<long long>(getAsDouble(valLeft)) + static_cast<long long>(getAsDouble(valRight));
                if (res > 2147483647LL || res < -2147483648LL) throw NumericalOverflowException();
                memory.push(static_cast<int>(res)); // Hasil Int
            }
            break;
        }
        case OprCode::SUB: {
            if (hasString) throw std::runtime_error("Runtime Error: Operator SUB tidak valid untuk string.");
            if (hasDouble) memory.push(getAsDouble(valLeft) - getAsDouble(valRight));
            else {
                long long res = static_cast<long long>(getAsDouble(valLeft)) - static_cast<long long>(getAsDouble(valRight));
                if (res > 2147483647LL || res < -2147483648LL) throw NumericalOverflowException();
                memory.push(static_cast<int>(res));
            }
            break;
        }
        case OprCode::MUL: {
            if (hasString) throw std::runtime_error("Runtime Error: Operator MUL tidak valid untuk string.");
            if (hasDouble) memory.push(getAsDouble(valLeft) * getAsDouble(valRight));
            else {
                long long res = static_cast<long long>(getAsDouble(valLeft)) * static_cast<long long>(getAsDouble(valRight));
                if (res > 2147483647LL || res < -2147483648LL) throw NumericalOverflowException();
                memory.push(static_cast<int>(res));
            }
            break;
        }
        case OprCode::DIV: {
            if (hasString || hasDouble) throw std::runtime_error("Runtime Error: Operator DIV (Integer) hanya untuk integer.");
            int r = static_cast<int>(getAsDouble(valRight));
            if (r == 0) throw std::runtime_error("Runtime Error: Division by zero.");
            memory.push(static_cast<int>(getAsDouble(valLeft)) / r); 
            break;
        }
        case OprCode::RDIV: {
            if (hasString) throw std::runtime_error("Runtime Error: Operator / tidak valid untuk string.");
            double r = getAsDouble(valRight);
            if (r == 0.0) throw std::runtime_error("Runtime Error: Division by zero.");
            memory.push(getAsDouble(valLeft) / r); 
            break;
        }
        case OprCode::MOD: {
            if (hasString || hasDouble) throw std::runtime_error("Runtime Error: Operator MOD hanya untuk integer.");
            int r = static_cast<int>(getAsDouble(valRight));
            if (r == 0) throw std::runtime_error("Runtime Error: Modulo by zero.");
            memory.push(static_cast<int>(getAsDouble(valLeft)) % r); 
            break;
        }
        case OprCode::EQL: {
            if (hasString) {
                std::string lStr = isString(valLeft) ? std::get<std::string>(valLeft) : "";
                std::string rStr = isString(valRight) ? std::get<std::string>(valRight) : "";
                memory.push(lStr == rStr ? 1 : 0);
            } else {
                memory.push(getAsDouble(valLeft) == getAsDouble(valRight) ? 1 : 0);
            }
            break;
        }
        case OprCode::NEQ: {
            if (hasString) {
                std::string lStr = isString(valLeft) ? std::get<std::string>(valLeft) : "";
                std::string rStr = isString(valRight) ? std::get<std::string>(valRight) : "";
                memory.push(lStr != rStr ? 1 : 0);
            } else {
                memory.push(getAsDouble(valLeft) != getAsDouble(valRight) ? 1 : 0);
            }
            break;
        }
        case OprCode::LSS: {
            if (hasString) {
                std::string lStr = isString(valLeft) ? std::get<std::string>(valLeft) : "";
                std::string rStr = isString(valRight) ? std::get<std::string>(valRight) : "";
                memory.push(lStr < rStr ? 1 : 0);
            } else {
                memory.push(getAsDouble(valLeft) < getAsDouble(valRight) ? 1 : 0);
            }
            break;
        }
        case OprCode::GEQ: {
            if (hasString) {
                std::string lStr = isString(valLeft) ? std::get<std::string>(valLeft) : "";
                std::string rStr = isString(valRight) ? std::get<std::string>(valRight) : "";
                memory.push(lStr >= rStr ? 1 : 0);
            } else {
                memory.push(getAsDouble(valLeft) >= getAsDouble(valRight) ? 1 : 0);
            }
            break;
        }
        case OprCode::GTR: {
            if (hasString) {
                std::string lStr = isString(valLeft) ? std::get<std::string>(valLeft) : "";
                std::string rStr = isString(valRight) ? std::get<std::string>(valRight) : "";
                memory.push(lStr > rStr ? 1 : 0);
            } else {
                memory.push(getAsDouble(valLeft) > getAsDouble(valRight) ? 1 : 0);
            }
            break;
        }
        case OprCode::LEQ: {
            if (hasString) {
                std::string lStr = isString(valLeft) ? std::get<std::string>(valLeft) : "";
                std::string rStr = isString(valRight) ? std::get<std::string>(valRight) : "";
                memory.push(lStr <= rStr ? 1 : 0);
            } else {
                memory.push(getAsDouble(valLeft) <= getAsDouble(valRight) ? 1 : 0);
            }
            break;
        }
        default:
            break;
    }
}