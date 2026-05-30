#include "Interpreter.hpp"
#include "ALU.hpp"
#include <iostream>

Interpreter::Interpreter() : ip_(0) {}

void Interpreter::execute(const std::vector<Instruction>& instructions) {
    ip_ = 0;
    memory_.clear(); // Kosongkan memori tiap eksekusi

    std::cout << "INTERPRETER MULAI MENG-INI-ITU-KAN" << std::endl;

    while (ip_ < instructions.size()) {
        // FETCH: Ambil instruksi saat ini
        Instruction instr = instructions[ip_];

        // Keknya nanti kalau udah siap hapus aja idk
        std::cout << "IP[" << ip_ << "] Exec: " << instr.toString(ip_) << " \t| ";

        // IP maju satu langkah sebelum instruksi dieksekusi
        ip_++; 

        // DECODE & EXECUTE: Pahami dan jalankan
        switch (instr.getOp()) {
            case OpCode::INT: memory_.allocate(instr.getOperand()); break;
            case OpCode::LIT: memory_.push(instr.getOperand()); break;
            case OpCode::STO: memory_.store(instr.getOperand()); break;
            case OpCode::LOD: memory_.load(instr.getOperand()); break;
            case OpCode::OPR: ALU::execute(static_cast<OprCode>(instr.getOperand()), memory_); break;
            // TODO: JMP, JPC, CAL, RET coming soon ya bang
            default: break;
        }

        memory_.printTrace(); // Print memori setelah 1 instruksi selesai (nanti reminder hapus juga ya bg)
    }
    std::cout << "INTERPRETER SELESAI MENG-INI-ITU-KAN" << std::endl;
}