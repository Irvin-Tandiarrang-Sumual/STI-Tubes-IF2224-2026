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

static std::string nonTerminalToString(NonTerminal nt) {
    switch (nt) {
        case NonTerminal::PROGRAM: return "program";
        case NonTerminal::PROGRAM_HEADER: return "program-header";
        case NonTerminal::DECLARATION_PART: return "declaration-part";
        case NonTerminal::CONST_DECLARATION: return "const-declaration";
        case NonTerminal::CONSTANT: return "constant";
        case NonTerminal::TYPE_DECLARATION: return "type-declaration";
        case NonTerminal::VAR_DECLARATION: return "var-declaration";
        case NonTerminal::IDENTIFIER_LIST: return "identifier-list";
        case NonTerminal::TYPE: return "type";
        case NonTerminal::ARRAY_TYPE: return "array-type";
        case NonTerminal::RANGE: return "range";
        case NonTerminal::ENUMERATED: return "enumerated";
        case NonTerminal::RECORD_TYPE: return "record-type";
        case NonTerminal::FIELD_LIST: return "field-list";
        case NonTerminal::FIELD_PART: return "field-part";
        case NonTerminal::SUBPROGRAM_DECLARATION: return "subprogram-declaration";
        case NonTerminal::PROCEDURE_DECLARATION: return "procedure-declaration";
        case NonTerminal::FUNCTION_DECLARATION: return "function-declaration";
        case NonTerminal::BLOCK: return "block";
        case NonTerminal::FORMAL_PARAMETER_LIST: return "formal-parameter-list";
        case NonTerminal::PARAMETER_GROUP: return "parameter-group";
        case NonTerminal::COMPOUND_STATEMENT: return "compound-statement";
        case NonTerminal::STATEMENT_LIST: return "statement-list";
        case NonTerminal::STATEMENT: return "statement";
        case NonTerminal::VARIABLE: return "variable";
        case NonTerminal::COMPONENT_VARIABLE: return "component-variable";
        case NonTerminal::INDEX_LIST: return "index-list";
        case NonTerminal::ASSIGNMENT_STATEMENT: return "assignment-statement";
        case NonTerminal::IF_STATEMENT: return "if-statement";
        case NonTerminal::CASE_STATEMENT: return "case-statement";
        case NonTerminal::CASE_BLOCK: return "case-block";
        case NonTerminal::WHILE_STATEMENT: return "while-statement";
        case NonTerminal::REPEAT_STATEMENT: return "repeat-statement";
        case NonTerminal::FOR_STATEMENT: return "for-statement";
        case NonTerminal::PROCEDURE_OR_FUNCTION_CALL: return "procedure/function-call";
        case NonTerminal::PARAMETER_LIST: return "parameter-list";
        case NonTerminal::EXPRESSION: return "expression";
        case NonTerminal::SIMPLE_EXPRESSION: return "simple-expression";
        case NonTerminal::TERM: return "term";
        case NonTerminal::FACTOR: return "factor";
        case NonTerminal::RELATIONAL_OPERATOR: return "relational-operator";
        case NonTerminal::ADDITIVE_OPERATOR: return "additive-operator";
        case NonTerminal::MULTIPLICATIVE_OPERATOR: return "multiplicative-operator";
        default: return "unknown";
    }
}

static std::string nodeToString(const CSTNodes* node) {
    if (node->isTerminal()) {
        const Token& token = node->getToken();
        std::string typeStr = tokenTypeToString(token.type);
        if (tokenNeedsValue(token.type)) {
            return typeStr + "(" + tokenValueToString(token) + ")";
        }
        return typeStr;
    }
    return "<" + nonTerminalToString(node->getNonTerminal()) + ">";
}

void Writer::writeTreeRecursive(std::ostream& out, const CSTNodes* node, const std::string& prefix, bool isLast) const {
    if (node == nullptr) {
        return;
    }
    out << prefix;
    if (!prefix.empty()) {
        if (isLast) {
            out << "└── ";
        } else {
            out << "├── ";
        }
    }

    out << nodeToString(node) << '\n';

    const auto& children = node->getChildren();

    for (size_t i = 0; i < children.size(); ++i) {
        bool childIsLast = (i == children.size() - 1);
        std::string childPrefix = prefix;

        if (!prefix.empty()) {
            if (isLast) {
                childPrefix += "    ";
            } else {
                childPrefix += "│   ";
            }
        }
        writeTreeRecursive(out, children[i], childPrefix, childIsLast);
    }
}

void Writer::printTree() const {
    if (root == nullptr) {
        std::cerr << "ERROR: root parse tree kosong.\n";
        return;
    }
    writeTreeRecursive(std::cout, root, "", true);
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
    writeTreeRecursive(fOut, root, "", true);
    fOut.close();

    std::cout << "Parse tree berhasil disimpan di: " << filename << "\n";
}
