#include "Reader.hpp"

Reader::Reader(const std::filesystem::path& path) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Can't open file path");
    }
    std::ostringstream ss;
    ss << inputFile.rdbuf();

    input_ = ss.str();

    advance();
    
}

bool Reader::isEOF() const {
    return index_ >= input_.size();
}

char Reader::getCurrentCharacter() const {
    if (isEOF()) {
        return '\0';
    }

    return currentCharacter;
}

void Reader::advance() {
    if (isEOF()) {
        return;
    }

    const char current = input_[index_++];
    if (current == '\n') {
        location_.line += 1;
        location_.col = 0;
    } 
    location_.col += 1;

    currentCharacter = current;
    return;
}

std::size_t Reader::getIndex() const {
    return index_;
}

CodeLocation Reader::getLocation() const {
    return location_;
}

void Reader::reset() {
    index_ = 0;
    location_ = {1ull, 0ull};
}
