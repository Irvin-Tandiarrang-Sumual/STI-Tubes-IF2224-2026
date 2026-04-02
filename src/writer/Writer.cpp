#include "Writer.hpp"

#include <algorithm>
#include <filesystem>

Writer::Writer(const std::string &filename, const std::vector<Token> &tokens)
    : filename(filename), tokens(tokens) {}

Writer::~Writer() = default;

void Writer::writeToFile() const {
    std::ofstream fOut(filename, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "ERROR: gagal membuka file output: " << filename << "\n";
        return;
    }

    unsigned int previousEndLine = 1;
    bool firstToken = true;

    for (const auto &token : tokens) {
        const unsigned int startLine = token.codeLocation.line;
        if (!firstToken && startLine > previousEndLine) {
            for (unsigned int i = 0; i < startLine - previousEndLine - 1; ++i) {
                fOut << '\n';
            }
        }

        const std::string typeStr = tokenTypeToString(token.type);
        if (tokenNeedsValue(token.type)) {
            fOut << typeStr << " (" << tokenValueToString(token) << ")\n";
        } else {
            fOut << typeStr << "\n";
        }

        unsigned int endLine = startLine;
        if (token.type == comment || token.type == string || token.type == charcon || token.type == invalid_token) {
            const std::string raw = tokenValueToString(token);
            endLine += static_cast<unsigned int>(std::count(raw.begin(), raw.end(), '\n'));
        }

        previousEndLine = endLine;
        firstToken = false;
    }

    fOut.close();

    if (std::filesystem::exists(filename)) {
        std::cout << "Output berhasil disimpan di: " << filename << "\n";
    }
}
