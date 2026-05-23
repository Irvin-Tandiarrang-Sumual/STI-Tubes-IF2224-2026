#include "SemanticAnalyzer.hpp"

void SemanticAnalyzer::initializePredefinedIdentifiers() {
    // dummy
    symbolTable.insertVariable("<dummy>", DataType::VOID);

    // (idx : 1-27)
    std::vector<std::string> keywords = {
        "and", "array", "begin", "case", "const", "div", "downto", "do", 
        "else", "end", "for", "function", "if", "mod", "not", "of", 
        "or", "procedure", "program", "record", "repeat", "then", "to", 
        "type", "until", "var", "while"
    };
    for (const auto& kw : keywords) {
        symbolTable.insertVariable(kw, DataType::VOID);
    }

    // predefined Types (Indeks 28 - 32)
    symbolTable.insertVariable("integer", DataType::INTEGER);
    symbolTable.insertVariable("real",    DataType::REAL);
    symbolTable.insertVariable("char",    DataType::CHAR);
    symbolTable.insertVariable("boolean", DataType::BOOLEAN);
    symbolTable.insertVariable("string",  DataType::STRING);
    // additional
    symbolTable.insertVariable("writeln", DataType::VOID);
    symbolTable.insertVariable("println",  DataType::VOID);
    // Kunci true & false biar gabisa diassign karena constant
    int trueIdx = symbolTable.insertVariable("true", DataType::BOOLEAN);
    IdentifierTableEntry& trueEntry = symbolTable.getIdentifier(trueIdx);
    trueEntry.normal = true;
    trueEntry.isConstant = true;
    int falseIdx = symbolTable.insertVariable("false", DataType::BOOLEAN);
    IdentifierTableEntry& falseEntry = symbolTable.getIdentifier(falseIdx);
    falseEntry.normal = true;
    falseEntry.isConstant = true;
}

void SemanticAnalyzer::predeclareSubprograms(const std::vector<ASTDeclarationNode*>& declarations) {
    auto& namesInScope = predeclaredSubprogramNames_[currentLevel];

    for (auto* decl : declarations) {
        if (decl == nullptr) {
            continue;
        }

        if (auto* procedureDecl = dynamic_cast<ASTProcedureDeclarationNode*>(decl)) {
            if (namesInScope.find(procedureDecl->name) != namesInScope.end()) {
                throw std::runtime_error("Semantic Error: Prosedur '" + procedureDecl->name + "' sudah dideklarasikan di scope ini!");
            }

            int existingIdx = symbolTable.lookup(procedureDecl->name);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Prosedur '" + procedureDecl->name + "' sudah dideklarasikan di scope ini!");
            }

            symbolTable.insertVariable(procedureDecl->name, DataType::VOID);
            namesInScope.insert(procedureDecl->name);
        } else if (auto* functionDecl = dynamic_cast<ASTFunctionDeclarationNode*>(decl)) {
            if (namesInScope.find(functionDecl->name) != namesInScope.end()) {
                throw std::runtime_error("Semantic Error: Fungsi '" + functionDecl->name + "' sudah dideklarasikan di scope ini!");
            }

            int existingIdx = symbolTable.lookup(functionDecl->name);
            if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
                throw std::runtime_error("Semantic Error: Fungsi '" + functionDecl->name + "' sudah dideklarasikan di scope ini!");
            }

            DataType returnType = resolveTypeNameKind(functionDecl->returnTypeName);
            symbolTable.insertVariable(functionDecl->name, returnType);
            namesInScope.insert(functionDecl->name);
        }
    }
}

std::any SemanticAnalyzer::visitProgramNode(ASTProgramNode* node) {
    // tar printout di luar
    std::cout << "Menganalisis Program: " << node->programName << std::endl;

    int existingIdx = symbolTable.lookup(node->programName);
    if (existingIdx != -1 && symbolTable.getIdentifier(existingIdx).level == currentLevel) {
        throw std::runtime_error("Semantic Error: Program '" + node->programName + "' sudah dideklarasikan di scope ini!");
    }

    int programIdx = symbolTable.insertVariable(node->programName, DataType::VOID);
    symbolTable.getIdentifier(programIdx).typeName = node->programName;

    node->symbolRefIndex_ = programIdx;
    node->lexicalLevel_ = currentLevel;
    node->evalType_ = DataType::VOID;

    safeVisitNode(node->declarations);
    safeVisitNode(node->mainBlock);
    return {};
}

