#include "CodeLocation.hpp"

std::string CodeLocation::print() const{
    return "Code is at line: " + std::to_string(line) + " and column: " + std::to_string(col) + "\n";
}
