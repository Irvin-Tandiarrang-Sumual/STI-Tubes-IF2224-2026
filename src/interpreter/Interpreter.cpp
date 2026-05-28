#include "Interpreter.hpp"
#include <iostream>

void Interpreter::execute(const std::vector<Instruction>& instructions) {
    std::cout << "Menjalankan VM... (Menerima " << instructions.size() << " instruksi), semoga kuat menjadi mesin" << std::endl;
}