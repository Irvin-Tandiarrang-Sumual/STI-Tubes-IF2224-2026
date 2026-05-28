#include "Interpreter.hpp"
#include <iostream>
#include <stdexcept>

void Interpreter::execute(const std::vector<Instruction>& instructions) {
    ip_ = 0; // Mulai dari 0 ya kak...
    basePtr_ = 0;
    stack_.clear(); // Kosongkan memori tiap eksekusi

    std::cout << "INTERPRETER MULAI MENG-INI-ITU-KAN" << std::endl;

    while (ip_ < instructions.size()) {
        // FETCH: Ambil instruksi saat ini
        Instruction instr = instructions[ip_];
        
        // IP maju satu langkah sebelum instruksi dieksekusi
        ip_++; 

        // DECODE & EXECUTE: Pahami dan jalankan
        switch (instr.getOp()) {
            case OpCode::LIT:
                // Push nilai literal (v) ke Stack
                // Contoh: LIT 0 2 -> Push value 2 ke stack
                stack_.push_back(instr.getOperand());
                break;
            case OpCode::LOD:
                // Copy nilai dari alamat (a) dan Push ke Stack
                // Alamat = basePtr_ + operand
                // Contoh: LOD 0 2
                // Before: [0, 1, 2, 3, 4]
                // After: [0, 1, 2, 3, 4, 2]
                stack_.push_back(stack_[basePtr_ + instr.getOperand()]);
                break;
            case OpCode::STO: {
                // Pop dan simpan ke alamat (a)
                // Contoh: STO 0 3
                // Before: [0, 1, 2, 3, 4, 10]
                // After: [0, 1, 2, 10, 4]
                int val = stack_.back();
                stack_.pop_back();
                stack_[basePtr_ + instr.getOperand()] = val;
                break;
            }
            case OpCode::INT:
                // Alokasikan memori sebesar operand (m)
                // Contoh: INT 0 5 -> Sediakan 5 ruang kosong (dengan value 0) di stack
                stack_.resize(stack_.size() + instr.getOperand(), 0);
                break;
            case OpCode::OPR: {
                OprCode opr = static_cast<OprCode>(instr.getOperand());
                
                // Operasi Unary
                if (opr == OprCode::NEG) {
                    int val = stack_.back();
                    stack_.pop_back();
                    stack_.push_back(-val);
                    break;
                }

                // Operasi Binary
                // Nilai operand kanan di-Pop terlebih dahulu
                int valRight = stack_.back(); stack_.pop_back();
                int valLeft = stack_.back(); stack_.pop_back();

                switch (opr) {
                    case OprCode::ADD: stack_.push_back(valLeft + valRight); break;
                    case OprCode::SUB: stack_.push_back(valLeft - valRight); break;
                    case OprCode::MUL: stack_.push_back(valLeft * valRight); break;
                    case OprCode::DIV: 
                        if (valRight == 0) throw std::runtime_error("Runtime Error: Division by zero.");
                        stack_.push_back(valLeft / valRight); 
                        break;
                    case OprCode::MOD: 
                        if (valRight == 0) throw std::runtime_error("Runtime Error: Modulo by zero.");
                        stack_.push_back(valLeft % valRight); 
                        break;
                    // TODO: Operasi Perbandingan (EQL, NEQ, LSS, GEQ, GTR, LEQ) & Penulisan (WRT, WRTLN)
                    default:
                        break;
                }
                break;
            }
            // TODO: JMP, JPC, CAL, RET coming soon ya bang
            default:
                break;
        }
    }
    std::cout << "INTERPRETER SELESAI MENG-INI-ITU-KAN" << std::endl;
}