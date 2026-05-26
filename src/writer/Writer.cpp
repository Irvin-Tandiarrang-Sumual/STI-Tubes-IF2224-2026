#include "Writer.hpp"

namespace {
std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::REAL: return "REAL";
        case DataType::INTEGER: return "INTEGER";
        case DataType::CHAR: return "CHAR";
        case DataType::BOOLEAN: return "BOOLEAN";
        case DataType::STRING: return "STRING";
        case DataType::RANGE: return "RANGE";
        case DataType::ENUMERATED: return "ENUMERATED";
        case DataType::ARRAY: return "ARRAY";
        case DataType::RECORD: return "RECORD";
        case DataType::VOID: return "VOID";
        default: return "UNKNOWN";
    }
}
}

Writer::Writer(const std::string &filename, const std::vector<Token> &tokens)
    : filename(filename), tokens(tokens) {}

Writer::~Writer() = default;

void Writer::writeTokenToFile() const {
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
Writer::Writer(const std::string &filename, CSTNodes* root, const std::vector<std::string>& parserErrorMessages) 
    : filename(filename), tokens(), root(root), errorMessages_(parserErrorMessages)  {}

void Writer::writeCSTRecursive(std::ostream& out, const CSTNodes* node, const std::string& prefix, bool isLast, std::size_t depth) const {
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

        writeCSTRecursive(out, children[i], childPrefix, childIsLast, depth + 1);
    }
}

void Writer::writeASTRecursive(std::ostream& out, const ASTNode* node, const std::string& prefix, bool isLast, std::size_t depth) const {
    if (node == nullptr) return;

    if (depth == 0) {
        out << node->toString() << '\n';
    } else {
        out << prefix << "|\n";
        out << prefix << (isLast ? "\\-- " : "+-- ");
        out << node->toString() << '\n';
    }

    const auto& children = node->children_;
    for (size_t i = 0; i < children.size(); ++i) {
        bool childIsLast = (i == children.size() - 1);
        std::string childPrefix = prefix;

        if (depth == 0) childPrefix = "  ";
        else childPrefix += (isLast ? "    " : "|   ");

        writeASTRecursive(out, children[i], childPrefix, childIsLast, depth + 1);
    }
}

std::string Writer::formatDecoratedNode(const ASTNode* node) const {
    if (node == nullptr) {
        return "";
    }

    std::string result = node->toString();
    result += " {type=" + dataTypeToString(node->evalType_);
    result += ", tab_index=" + std::to_string(node->symbolRefIndex_);
    result += ", lev=" + std::to_string(node->lexicalLevel_);
    result += "}";
    return result;
}

void Writer::writeDecoratedASTRecursive(std::ostream& out, const ASTNode* node, const std::string& prefix, bool isLast, std::size_t depth) const {
    if (node == nullptr) return;

    if (depth == 0) {
        out << formatDecoratedNode(node) << '\n';
    } else {
        out << prefix << "|\n";
        out << prefix << (isLast ? "\\-- " : "+-- ");
        out << formatDecoratedNode(node) << '\n';
    }

    const auto& children = node->children_;
    for (size_t i = 0; i < children.size(); ++i) {
        bool childIsLast = (i == children.size() - 1);
        std::string childPrefix = prefix;

        if (depth == 0) childPrefix = "  ";
        else childPrefix += (isLast ? "    " : "|   ");

        writeDecoratedASTRecursive(out, children[i], childPrefix, childIsLast, depth + 1);
    }
}

void Writer::printAST(const ASTNode* rootAst) const {
    if (rootAst == nullptr) {
        std::cerr << "ERROR: root AST kosong.\n";
        return;
    }
    writeASTRecursive(std::cout, rootAst, "", true, 0);
}

void Writer::writeASTToFile(const ASTNode* rootAst) const {
    if (rootAst == nullptr) {
        std::cerr << "ERROR: root AST kosong.\n";
        return;
    }

    std::ofstream fOut(filename, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "ERROR: gagal membuka file output: " << filename << "\n";
        return;
    }
    writeASTRecursive(fOut, rootAst, "", true, 0);
    fOut.close();
    std::cout << "AST berhasil disimpan di: " << filename << "\n";
}

void Writer::printDecoratedAST(const ASTNode* rootAst) const {
    if (rootAst == nullptr) {
        std::cerr << "ERROR: root AST kosong.\n";
        return;
    }
    writeDecoratedASTRecursive(std::cout, rootAst, "", true, 0);
}

void Writer::writeDecoratedASTToFile(const ASTNode* rootAst) const {
    if (rootAst == nullptr) {
        std::cerr << "ERROR: root AST kosong.\n";
        return;
    }

    std::ofstream fOut(filename, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "ERROR: gagal membuka file output: " << filename << "\n";
        return;
    }
    writeDecoratedASTRecursive(fOut, rootAst, "", true, 0);
    fOut.close();
    std::cout << "Decorated AST berhasil disimpan di: " << filename << "\n";
}

void Writer::printDecoratedASTWithTables(const ASTNode* rootAst, const std::string& tablesText) const {
    printDecoratedAST(rootAst);
    std::cout << "\n=== Symbol Tables ===\n";
    std::cout << tablesText << std::endl;
}

void Writer::writeDecoratedASTWithTablesToFile(const ASTNode* rootAst, const std::string& tablesText) const {
    if (rootAst == nullptr) {
        std::cerr << "ERROR: root AST kosong.\n";
        return;
    }

    std::ofstream fOut(filename, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "ERROR: gagal membuka file output: " << filename << "\n";
        return;
    }

    writeDecoratedASTRecursive(fOut, rootAst, "", true, 0);
    fOut << "\n=== Symbol Tables ===\n";
    fOut << tablesText;
    if (!tablesText.empty() && tablesText.back() != '\n') {
        fOut << '\n';
    }

    fOut.close();
    std::cout << "Semantic report berhasil disimpan di: " << filename << "\n";
}

void Writer::printCST() const {
    if (root == nullptr) {
        std::cerr << "ERROR: root parse tree kosong.\n";
        return;
    }
    writeCSTRecursive(std::cout, root, "", true, 0);
}

void Writer::printParserError() const {
    std::cout << "\n=== Parser Errors ===\n";

    if (errorMessages_.empty()) {
        std::cout << "Tidak Terdapat Error pada Tahapan Parser!\n\n";
        return;
    }
    for (size_t i = 0; i < errorMessages_.size(); i++) {
        std::cout << errorMessages_.at(i) + "\n";
    }
    
}

void Writer::writeCSTToFile() const {
    if (root == nullptr) {
        std::cerr << "ERROR: root parse tree kosong.\n";
        return;
    }

    std::ofstream fOut(filename, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "ERROR: gagal membuka file output: " << filename << "\n";
        return;
    }
    writeCSTRecursive(fOut, root, "", true, 0);
    fOut.close();

    std::cout << "Parse tree berhasil disimpan di: " << filename << "\n";
}
