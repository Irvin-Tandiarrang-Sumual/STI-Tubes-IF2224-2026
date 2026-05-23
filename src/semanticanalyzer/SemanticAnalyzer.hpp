#pragma once

#include <any>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "../ast/ASTVisitor.hpp"
#include "../ast/ASTNodes.hpp"
#include "../symboltable/Entries.hpp"
#include "../symboltable/SymbolTable.hpp"

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable symbolTable;
    int currentLevel = 0;
    std::vector<std::string> errors_;

    std::vector<std::string> warnings_;
    enum class ConstantBoolResult {
        Unknown,
        AlwaysTrue,
        AlwaysFalse
    };
    ConstantBoolResult evaluateConstantBoolean(ASTExpressionNode* expression);
    bool extractConstantNumber(ASTExpressionNode* expression, double& outValue);
    bool extractConstantBoolean(ASTExpressionNode* expression, bool& outValue);

    std::unordered_map<std::string, ASTTypeNode*> namedTypeDefinitions_;
    std::unordered_map<int, ASTTypeNode*> identifierTypeNodes_;
    std::unordered_map<int, std::unordered_set<std::string>> predeclaredSubprogramNames_;
    std::unordered_map<const ASTArrayTypeNode*, int> arrayTypeEntries_;

    using RangeBound = std::variant<int, char, bool>;
    std::unordered_map<const ASTRangeType*, std::pair<RangeBound, RangeBound>> rangeBounds_;

    void reportError(const std::string& message);
    void reportError(const ASTNode* node, const std::string& message);

    void reportWarning(const std::string& message);
    void reportWarning(const ASTNode* node, const std::string& message);

    void safeVisitNode(ASTNode* node);

    DataType mapStringToDataType(const std::string& typeStr);
    ASTTypeNode* resolveTypeNode(ASTTypeNode* node);
    DataType resolveTypeKind(ASTTypeNode* node);
    ASTTypeNode* getStoredTypeNode(int symbolIndex);
    DataType resolveExpressionKind(ASTExpressionNode* expression);
    DataType resolveTypeNameKind(const std::string& typeName);
    void rememberIdentifierType(int symbolIndex, ASTTypeNode* typeNode);
    int literalToInt(const ASTLiteralExpressionNode* literal) const;
    bool extractRangeBounds(const ASTRangeType* range, int& low, int& high) const;
    int estimateTypeStorageSize(ASTTypeNode* typeNode);
    int ensureArrayTypeEntry(ASTArrayTypeNode* node);
    void predeclareSubprograms(const std::vector<ASTDeclarationNode*>& declarations);

    bool isNumericKind(DataType type) const;
    bool isOrdinalKind(DataType type) const;
    bool compareRangeLiteral(const ASTLiteralExpressionNode* literal, const ASTRangeType* range) const;
    bool areNamedRecordsCompatible(ASTTypeNode* lhsNode, ASTTypeNode* rhsNode, const IdentifierTableEntry& lhsEntry, const IdentifierTableEntry& rhsEntry) const;
    bool areTypesCompatible(ASTTypeNode* lhsNode, ASTTypeNode* rhsNode, DataType lhsKind, DataType rhsKind);
    void initializePredefinedIdentifiers();
    void validateAssignmentCompatibility(ASTExpressionNode* target, ASTExpressionNode* value, ASTTypeNode* targetTypeNode, ASTTypeNode* valueTypeNode, DataType targetType, DataType valueType);

public:
    SemanticAnalyzer();

    void analyze(ASTProgramNode* root);
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    bool hasErrors() const;
    std::string dumpTables() const;

    std::any visitProgramNode(ASTProgramNode* node) override;
    std::any visitDeclarationsNode(ASTDeclarationsNode* node) override;

    std::any visitTypeDeclarationNode(ASTTypeDeclarationNode* node) override;
    std::any visitConstDeclarationNode(ASTConstDeclarationNode* node) override;
    std::any visitVarDeclarationNode(ASTVarDeclarationNode* node) override;

    std::any visitPrimitiveType(ASTPrimitiveType* node) override;
    std::any visitNamedTypeNode(ASTNamedTypeNode* node) override;
    std::any visitRangeType(ASTRangeType* node) override;
    std::any visitArrayTypeNode(ASTArrayTypeNode* node) override;
    std::any visitRecordTypeNode(ASTRecordTypeNode* node) override;
    std::any visitEnumeratedTypeNode(ASTEnumeratedTypeNode* node) override;

    std::any visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node) override;
    std::any visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node) override;

    std::any visitUnaryExpressionNode(ASTUnaryExpressionNode* node) override;
    std::any visitVariableExpressionNode(ASTVariableExpressionNode* node) override;
    std::any visitLiteralExpressionNode(ASTLiteralExpressionNode* node) override;
    std::any visitBinaryExpressionNode(ASTBinaryExpressionNode* node) override;
    std::any visitCallExpressionNode(ASTCallExpressionNode* node) override;

    std::any visitAssignmentStatementNode(ASTAssignmentStatementNode* node) override;
    std::any visitIfStatementNode(ASTIfStatementNode* node) override;
    std::any visitBlockStatementNode(ASTBlockStatementNode* node) override;
    std::any visitWhileStatementNode(ASTWhileStatementNode* node) override;
    std::any visitRepeatStatementNode(ASTRepeatStatementNode* node) override;
    std::any visitForStatementNode(ASTForStatementNode* node) override;
    std::any visitCaseStatementNode(ASTCaseStatementNode* node) override;
    std::any visitCallStatementNode(ASTCallStatementNode* node) override;
};
