#pragma once

#include "ASTNodes.hpp"
#include "../cst/CSTNodes.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
class ASTBuilder {
    public:
        ASTBuilder() = default;

        // Entry point: expects root <program> CST node.
        std::unique_ptr<ASTProgramNode> build(const CSTNodes* root);

    private:

        // General helpers
        std::string tokenText(const CSTNodes* node) const;
        std::string operatorText(TokenType type) const;

        // Program structure
        std::unique_ptr<ASTProgramNode> buildProgram(const CSTNodes* node);
        std::string buildProgramHeader(const CSTNodes* node);
        void buildDeclarationPart(const CSTNodes* node, std::vector<std::unique_ptr<ASTDeclarationNode>>& out);
        void buildBlock(const CSTNodes* node, std::vector<std::unique_ptr<ASTDeclarationNode>>& localDeclarations, std::unique_ptr<ASTBlockStatementNode>& body);

        // Declarations
        void buildConstDeclaration(const CSTNodes* node, std::vector<std::unique_ptr<ASTDeclarationNode>>& out);
        void buildTypeDeclaration(const CSTNodes* node, std::vector<std::unique_ptr<ASTDeclarationNode>>& out);
        void buildVarDeclaration(const CSTNodes* node, std::vector<std::unique_ptr<ASTDeclarationNode>>& out);
        std::unique_ptr<ASTDeclarationNode> buildSubprogramDeclaration(const CSTNodes* node);
        std::unique_ptr<ASTDeclarationNode> buildProcedureDeclaration(const CSTNodes* node);
        std::unique_ptr<ASTDeclarationNode> buildFunctionDeclaration(const CSTNodes* node);

        // Types
        std::unique_ptr<ASTTypeNode> buildType(const CSTNodes* node);
        std::unique_ptr<ASTTypeNode> buildTypeFromIdentifier(const std::string& name, const CSTNodes* source);
        std::unique_ptr<ASTArrayTypeNode> buildArrayType(const CSTNodes* node);
        std::unique_ptr<ASTRangeType> buildRangeType(const CSTNodes* node);
        std::unique_ptr<ASTEnumeratedTypeNode> buildEnumeratedType(const CSTNodes* node);
        std::unique_ptr<ASTRecordTypeNode> buildRecordType(const CSTNodes* node);
        std::vector<ASTRecordFieldNode> buildFieldList(const CSTNodes* node);
        ASTRecordFieldNode buildFieldPart(const CSTNodes* node);
        std::vector<std::string> buildIdentifierList(const CSTNodes* node);

        // Parameters
        std::vector<ASTParameterGroup> buildFormalParameterList(const CSTNodes* node);
        ASTParameterGroup buildParameterGroup(const CSTNodes* node);
        std::vector<std::unique_ptr<ASTExpressionNode>> buildParameterList(const CSTNodes* node);

        // Statements
        std::unique_ptr<ASTBlockStatementNode> buildCompoundStatement(const CSTNodes* node);
        std::vector<std::unique_ptr<ASTStatementNode>> buildStatementList(const CSTNodes* node);
        std::unique_ptr<ASTStatementNode> buildStatement(const CSTNodes* node);
        std::unique_ptr<ASTAssignmentStatementNode> buildAssignmentStatement(const CSTNodes* node);
        std::unique_ptr<ASTStatementNode> buildIfStatement(const CSTNodes* node);
        std::unique_ptr<ASTCaseStatementNode> buildCaseStatement(const CSTNodes* node);
        void collectCaseBranches(const CSTNodes* node, std::vector<ASTCaseBranchNode>& out);
        std::unique_ptr<ASTWhileStatementNode> buildWhileStatement(const CSTNodes* node);
        std::unique_ptr<ASTRepeatStatementNode> buildRepeatStatement(const CSTNodes* node);
        std::unique_ptr<ASTForStatementNode> buildForStatement(const CSTNodes* node);
        std::unique_ptr<ASTCallExpressionNode> buildProcedureOrFunctionCall(const CSTNodes* node);

        // Variables
        std::unique_ptr<ASTVariableExpressionNode> buildVariable(const CSTNodes* node);
        ASTVariableComponent buildComponentVariable(const CSTNodes* node);
        std::vector<std::unique_ptr<ASTExpressionNode>> buildIndexList(const CSTNodes* node);

        // Expressions
        std::unique_ptr<ASTExpressionNode> buildExpression(const CSTNodes* node);
        std::unique_ptr<ASTExpressionNode> buildSimpleExpression(const CSTNodes* node);
        std::unique_ptr<ASTExpressionNode> buildTerm(const CSTNodes* node);
        std::unique_ptr<ASTExpressionNode> buildFactor(const CSTNodes* node);
        std::unique_ptr<ASTExpressionNode> buildConstant(const CSTNodes* node);
        std::unique_ptr<ASTExpressionNode> buildLiteralOrIdentifierExpression(const CSTNodes* tokenNode);
};