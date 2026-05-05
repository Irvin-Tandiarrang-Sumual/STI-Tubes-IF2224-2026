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
        if (token.type == comment || token.type == string || token.type == charcon || token.type == unknown) {
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

// BUAT PARSER
Writer::Writer(const std::string &filename, CSTNodes* root) : filename(filename), tokens(), root(root) {}

void Writer::writeTreeRecursive(std::ostream& out, const CSTNodes* node, const std::string& prefix, bool isLast, std::size_t depth) const {
    if (node == nullptr) {
        return;
    }

    if (depth == 0) {
        out << node->toString() << '\n';
    } else {
        out << prefix;
        out << (isLast ? "└── " : "├── ");
        out << node->toString() << '\n';
    }

    const auto& children = node->getChildren();

    for (size_t i = 0; i < children.size(); ++i) {
        bool childIsLast = (i == children.size() - 1);
        std::string childPrefix = prefix;

        if (depth == 0) {
            childPrefix.clear();
        } else {
            childPrefix += (isLast ? "    " : "│   ");
        }

        writeTreeRecursive(out, children[i], childPrefix, childIsLast, depth + 1);
    }
}

void Writer::printTree() const {
    if (root == nullptr) {
        std::cerr << "ERROR: root parse tree kosong.\n";
        return;
    }
    writeTreeRecursive(std::cout, root, "", true, 0);
}

void Writer::writeTreeToFile() const {
    if (root == nullptr) {
        std::cerr << "ERROR: root parse tree kosong.\n";
        return;
    }

    std::ofstream fOut(filename, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "ERROR: gagal membuka file output: " << filename << "\n";
        return;
    }
    writeTreeRecursive(fOut, root, "", true, 0);
    fOut.close();

    std::cout << "Parse tree berhasil disimpan di: " << filename << "\n";
}
