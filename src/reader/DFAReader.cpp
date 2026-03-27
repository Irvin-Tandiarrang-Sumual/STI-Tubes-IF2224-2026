#include "DFAReader.hpp"

DFAReader::DFAReader(std::string input)
    : input_(std::move(input)), index_(0), location_{1, 1} {}

bool DFAReader::eof() const {
    return index_ >= input_.size();
}

char DFAReader::peek() const {
    if (eof()) {
        return '\0';
    }

    return input_[index_];
}

char DFAReader::get() {
    if (eof()) {
        return '\0';
    }

    const char current = input_[index_++];
    if (current == '\n') {
        location_.line += 1;
        location_.col = 1;
    } else {
        location_.col += 1;
    }

    return current;
}

std::size_t DFAReader::position() const {
    return index_;
}

CodeLocation DFAReader::location() const {
    return location_;
}

void DFAReader::reset() {
    index_ = 0;
    location_ = {1, 1};
}
