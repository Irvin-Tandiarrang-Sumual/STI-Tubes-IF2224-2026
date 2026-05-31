#include "Interpreter.hpp"
#include "ALU.hpp"
#include <iostream>

Interpreter::Interpreter() : ip_(0) {}

void Interpreter::execute(const std::vector<Instruction>& instructions, std::ostream& outStream) {
    ip_ = 0;
    memory_.clear(); // Kosongkan memori tiap eksekusi

    while (ip_ < static_cast<int>(instructions.size())) {
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
            case OpCode::STO: memory_.store(instr.getLevel(), instr.getOperand()); break;
            case OpCode::LOD: memory_.load(instr.getLevel(), instr.getOperand()); break;
            case OpCode::OPR: ALU::execute(static_cast<OprCode>(instr.getOperand()), memory_, outStream); break;
            case OpCode::JMP: 
                if (instr.getOperand() < 0 || instr.getOperand() >= static_cast<int>(instructions.size())) throw InvalidJumpTargetException();
                ip_ = instr.getOperand(); 
                break;
            case OpCode::JPC: 
                // Pop satu nilai, jika 0 (false), maka lompat
                if (memory_.pop() == 0) {
                    if (instr.getOperand() < 0 || instr.getOperand() >= static_cast<int>(instructions.size())) throw InvalidJumpTargetException();
                    ip_ = instr.getOperand();
                }
                break;
            case OpCode::CAL: {
                if (instr.getOperand() < 0 || instr.getOperand() >= static_cast<int>(instructions.size())) throw InvalidJumpTargetException();
                int sl = memory_.resolveBase(instr.getLevel()); // Static Link 
                int dl = memory_.getBasePtr();                  // Dynamic Link 
                int ra = ip_;                                   // Return Address 
                
                int newBase = memory_.getSize();                // Frame baru dimulai di ujung stack
                
                // Bangun Frame Header (3 slot)
                memory_.push(sl);
                memory_.push(dl);
                memory_.push(ra);
                
                memory_.setBasePtr(newBase);                    // Pindahkan Base Pointer ke frame baru
                ip_ = instr.getOperand();                       // Lompat ke subprogram
                break;
            }
            case OpCode::RET: {
                int oldBase = memory_.getBasePtr();
                
                if (oldBase == 0) {
                    // Jika return dari main program, hentikan eksekusi
                    ip_ = instructions.size(); 
                } else {
                    // Selalu meletakkan nilai return di ujung Stack sebelum RET
                    int returnValue = memory_.pop(); 
                    
                    int ra = memory_.get(oldBase + 2); // Ambil Return Address
                    int dl = memory_.get(oldBase + 1); // Ambil Dynamic Link
                    
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

        memory_.printTrace(); // Print memori setelah 1 instruksi selesai (nanti reminder hapus juga ya bg)
    }
}