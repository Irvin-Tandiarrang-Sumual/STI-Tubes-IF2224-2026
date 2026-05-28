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
                // TODO: Implementasi LIT
                break;
            case OpCode::LOD:
                // TODO: Implementasi LOD
                break;
            case OpCode::STO:
                // TODO: Implementasi STO
                break;
            case OpCode::INT:
                // TODO: Implementasi INT
                break;
            case OpCode::OPR:
                // TODO: Implementasi OPR
                break;
            // JMP, JPC, CAL, RET coming soon ya bang
            default:
                break;
        }
    }
    std::cout << "INTERPRETER SELESAI MENG-INI-ITU-KAN" << std::endl;
}