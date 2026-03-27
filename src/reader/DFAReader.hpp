#pragma once

#include <cstddef>
#include <string>

#include "CodeLocation.hpp"

class DFAReader {
public:
    explicit DFAReader(std::string input);

    bool eof() const;
    char peek() const;
    char get();

    std::size_t position() const;
    CodeLocation location() const;

    void reset();

private:
    std::string input_;
    std::size_t index_;
    CodeLocation location_;
};
