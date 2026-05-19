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
        ASTProgramNode* build(const CSTNodes* root);

    private:

        // General helpers
        std::string tokenText(const CSTNodes* node) const;
        std::string operatorText(TokenType type) const;

        // Program structure
        ASTProgramNode* buildProgram(const CSTNodes* node);
        std::string buildProgramHeader(const CSTNodes* node);
        void buildDeclarationPart(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out);
        void buildBlock(const CSTNodes* node, std::vector<ASTDeclarationNode*>& localDeclarations, ASTBlockStatementNode*& body);

        // Declarations
        void buildConstDeclaration(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out);
        void buildTypeDeclaration(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out);
        void buildVarDeclaration(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out);
        ASTDeclarationNode* buildSubprogramDeclaration(const CSTNodes* node);
        ASTDeclarationNode* buildProcedureDeclaration(const CSTNodes* node);
        ASTDeclarationNode* buildFunctionDeclaration(const CSTNodes* node);

        // Types
        ASTTypeNode* buildType(const CSTNodes* node);
        ASTTypeNode* buildTypeFromIdentifier(const std::string& name, const CSTNodes* source);
        ASTArrayTypeNode* buildArrayType(const CSTNodes* node);
        ASTRangeType* buildRangeType(const CSTNodes* node);
        ASTEnumeratedTypeNode* buildEnumeratedType(const CSTNodes* node);
        ASTRecordTypeNode* buildRecordType(const CSTNodes* node);
        std::vector<ASTRecordFieldNode> buildFieldList(const CSTNodes* node);
        ASTRecordFieldNode buildFieldPart(const CSTNodes* node);
        std::vector<std::string> buildIdentifierList(const CSTNodes* node);

        // Parameters
        std::vector<ASTParameterGroup> buildFormalParameterList(const CSTNodes* node);
        ASTParameterGroup buildParameterGroup(const CSTNodes* node);
        std::vector<ASTExpressionNode*> buildParameterList(const CSTNodes* node);

        // Statements
        ASTBlockStatementNode* buildCompoundStatement(const CSTNodes* node);
        std::vector<ASTStatementNode*> buildStatementList(const CSTNodes* node);
        ASTStatementNode* buildStatement(const CSTNodes* node);
        ASTAssignmentStatementNode* buildAssignmentStatement(const CSTNodes* node);
        ASTStatementNode* buildIfStatement(const CSTNodes* node);
        ASTCaseStatementNode* buildCaseStatement(const CSTNodes* node);
        void collectCaseBranches(const CSTNodes* node, std::vector<ASTCaseBranchNode>& out);
        ASTWhileStatementNode* buildWhileStatement(const CSTNodes* node);
        ASTRepeatStatementNode* buildRepeatStatement(const CSTNodes* node);
        ASTForStatementNode* buildForStatement(const CSTNodes* node);
        ASTCallExpressionNode* buildProcedureOrFunctionCall(const CSTNodes* node);

        // Variables
        ASTVariableExpressionNode* buildVariable(const CSTNodes* node);
        ASTVariableComponent buildComponentVariable(const CSTNodes* node);
        std::vector<ASTExpressionNode*> buildIndexList(const CSTNodes* node);

        // Expressions
        ASTExpressionNode* buildExpression(const CSTNodes* node);
        ASTExpressionNode* buildSimpleExpression(const CSTNodes* node);
        ASTExpressionNode* buildTerm(const CSTNodes* node);
        ASTExpressionNode* buildFactor(const CSTNodes* node);
        ASTExpressionNode* buildConstant(const CSTNodes* node);
        ASTExpressionNode* buildLiteralOrIdentifierExpression(const CSTNodes* tokenNode);
};