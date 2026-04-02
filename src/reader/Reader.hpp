#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "CodeLocation.hpp"

class Reader {
public:
    explicit Reader(const std::filesystem::path &path);

    bool isEOF() const;
    char getCurrentCharacter() const;
    char peek(std::size_t offset = 1) const;
    void advance();

    std::size_t getIndex() const;
    CodeLocation getLocation() const;
    void reset();

private:
    std::string input_;
    std::size_t index_ = 0;
    unsigned int line_ = 1;
    unsigned int col_ = 1;
};
