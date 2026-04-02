#pragma once

#include <string>

struct CodeLocation {
    unsigned int line = 1;
    unsigned int col = 1;

    std::string print() const;
};
