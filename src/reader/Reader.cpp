#include "Reader.hpp"

Reader::Reader(const std::filesystem::path &path) {
    std::ifstream inputFile(path, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Tidak bisa membuka file input: " + path.string());
    }

    std::ostringstream ss;
    ss << inputFile.rdbuf();
    input_ = ss.str();
}

bool Reader::isEOF() const {
    return index_ >= input_.size();
}

char Reader::getCurrentCharacter() const {
    return isEOF() ? '\0' : input_[index_];
}

char Reader::peek(std::size_t offset) const {
    const std::size_t target = index_ + offset;
    return (target < input_.size()) ? input_[target] : '\0';
}

void Reader::advance() {
    if (isEOF()) {
        return;
    }

    if (input_[index_] == '\n') {
        ++line_;
        col_ = 1;
    } else {
        ++col_;
    }

    ++index_;
}

std::size_t Reader::getIndex() const {
    return index_;
}

CodeLocation Reader::getLocation() const {
    return CodeLocation{line_, col_};
}

void Reader::reset() {
    index_ = 0;
    line_ = 1;
    col_ = 1;
}
