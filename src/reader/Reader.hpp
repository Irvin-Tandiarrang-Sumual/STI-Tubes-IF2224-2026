#pragma once

#include <cstddef>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "CodeLocation.hpp"

class Reader {
    public:
        explicit Reader(const std::filesystem::path& path);

        bool isEOF() const;
        char getCurrentCharacter() const;
        void advance();
        char peekNextChar(); // lihat 1 char setelahnya (ga pindah tempat)

        std::size_t getIndex() const;
        CodeLocation getLocation() const;

        void reset();

    private:
        char currentCharacter = '\0';
        std::string input_; // using string or ifstream(???)
        std::size_t index_ = 0;
        CodeLocation location_ = {1ull, 0ull};
};
