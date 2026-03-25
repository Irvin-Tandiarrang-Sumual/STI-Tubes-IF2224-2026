#pragma once
#include <iostream>
#include <string>

struct CodeLocation {
    unsigned int line;
    unsigned int col;
    
    std::string print() const;
};
