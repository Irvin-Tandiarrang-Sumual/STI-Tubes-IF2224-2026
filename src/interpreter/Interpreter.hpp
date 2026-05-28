#pragma once
#include <vector>
#include "../intermediate/Instruction.hpp"

class Interpreter {
private:
    std::vector<int> stack_; // Memori utama VM
    int ip_;                 // Instruction Pointer
    int basePtr_;            // Base pointer untuk Stack Frame / Dynamic Link
    
public:
    Interpreter() : ip_(0), basePtr_(0) {}
    
    // Menjalankan kumpulan instruksi dari CodeGenerator
    void execute(const std::vector<Instruction>& instructions); 
};