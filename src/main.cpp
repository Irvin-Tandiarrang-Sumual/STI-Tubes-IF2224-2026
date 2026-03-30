#include "Compiler.hpp"
#include <ostream>

int main () {
    std::cout << "Hii!\n";
    Compiler compiler("tc-1-spek.txt");
    std::cout << "I'm here after compiler initialization\n";
    compiler.lexer();
    std::cout << "I'm here after compiler compiler.lexer\n";

    return 0;
}