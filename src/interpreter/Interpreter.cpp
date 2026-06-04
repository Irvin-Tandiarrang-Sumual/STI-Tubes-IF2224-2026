#include "Interpreter.hpp"
#include "ALU.hpp"
#include <iostream>
#include <variant>

Interpreter::Interpreter() : ip_(0) {}

void Interpreter::execute(const std::vector<Instruction>& instructions, std::ostream& outStream) {
    ip_ = 0;
    memory_.clear(); // Kosongkan memori tiap eksekusi

    while (ip_ < static_cast<int>(instructions.size())) {
        // FETCH: Ambil instruksi saat ini
        Instruction instr = instructions[ip_];

        // IP maju satu langkah sebelum instruksi dieksekusi
        ip_++; 

        // DECODE & EXECUTE: Pahami dan jalankan
        switch (instr.getOp()) {
            case OpCode::INT: memory_.allocate(std::get<int>(instr.getOperand())); break;
            case OpCode::LIT: memory_.push(instr.getOperand()); break;
            case OpCode::STO: memory_.store(instr.getLevel(), std::get<int>(instr.getOperand())); break;
            case OpCode::LOD: memory_.load(instr.getLevel(), std::get<int>(instr.getOperand())); break;
            case OpCode::OPR: ALU::execute(static_cast<OprCode>(std::get<int>(instr.getOperand())), memory_, outStream); break;
            case OpCode::JMP: 
                if (std::get<int>(instr.getOperand()) < 0 || std::get<int>(instr.getOperand()) >= static_cast<int>(instructions.size())) throw InvalidJumpTargetException();
                ip_ = std::get<int>(instr.getOperand()); 
                break;
            case OpCode::JPC: {
                // Pop satu nilai, jika 0 (false), maka lompat
                auto condition = memory_.pop();
                if (std::holds_alternative<int>(condition) && std::get<int>(condition) == 0) {
                    if (std::get<int>(instr.getOperand()) < 0 || std::get<int>(instr.getOperand()) >= static_cast<int>(instructions.size())) throw InvalidJumpTargetException();
                    ip_ = std::get<int>(instr.getOperand());
                }
                break;
            }
            case OpCode::CAL: {
                if (std::get<int>(instr.getOperand()) < 0 || std::get<int>(instr.getOperand()) >= static_cast<int>(instructions.size())) throw InvalidJumpTargetException();
                int sl = memory_.resolveBase(instr.getLevel()); // Static Link 
                int dl = memory_.getBasePtr();                  // Dynamic Link 
                int ra = ip_;                                   // Return Address 
                
                int newBase = memory_.getSize();                // Frame baru dimulai di ujung stack
                
                // Bangun Frame Header (3 slot)
                memory_.push(sl);
                memory_.push(dl);
                memory_.push(ra);
                
                memory_.setBasePtr(newBase);                    // Pindahkan Base Pointer ke frame baru
                ip_ = std::get<int>(instr.getOperand());        // Lompat ke subprogram
                break;
            }
            case OpCode::RET: {
                int oldBase = memory_.getBasePtr();
                
                if (oldBase == 0) {
                    // Jika return dari main program, hentikan eksekusi
                    ip_ = static_cast<int>(instructions.size()); 
                } else {
                    // Selalu meletakkan nilai return di ujung Stack sebelum RET
                    auto returnValue = memory_.pop(); 
                    
                    int ra = std::get<int>(memory_.get(oldBase + 2)); // Ambil Return Address
                    int dl = std::get<int>(memory_.get(oldBase + 1)); // Ambil Dynamic Link
                    
                    memory_.setSize(oldBase);          // Hapus Stack Frame
                    memory_.setBasePtr(dl);            // Kembalikan Base Pointer pemanggil
                    
                    // Kembalikan nilai return ke atas stack agar bisa ditangkap oleh caller
                    memory_.push(returnValue);
                    
                    ip_ = ra;                          // Kembali ke baris instruksi pemanggil
                }
                break;
            }
            default: break;
        }
    }
}