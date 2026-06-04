#pragma once
#include <iostream>
#include "../intermediate/Instruction.hpp"
#include "RuntimeStack.hpp"

class ALU {
public:
    // Menerima operasi apa yang harus dilakukan, dan referensi ke memori
    static void execute(OprCode opr, RuntimeStack& memory, std::ostream& outStream = std::cout);
};