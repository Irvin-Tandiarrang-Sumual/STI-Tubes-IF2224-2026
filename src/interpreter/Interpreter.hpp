#pragma once
#include <vector>
#include "../intermediate/Instruction.hpp"
#include "RuntimeStack.hpp" // Include memori

class Interpreter {
private:
    RuntimeStack memory_; // Memori & Pengelolanya
    int ip_;              // Instruction Pointer
    
public:
    Interpreter();

    // Menjalankan kumpulan instruksi dari CodeGenerator
    void execute(const std::vector<Instruction>& instructions); 
};