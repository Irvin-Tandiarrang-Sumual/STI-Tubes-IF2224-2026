#pragma once
#include "../ast/ASTVisitor.hpp"

class TypeChecker : public ASTVisitor {
    public:
        TypeChecker();
        ~TypeChecker();
        std::any visitProgramNode(ASTProgramNode* node) override;

        std::any visitTypeDeclarationNode(ASTTypeDeclarationNode* node) override;
        std::any visitVarDeclarationNode(ASTVarDeclarationNode* node) override;
        std::any visitConstDeclarationNode(ASTConstDeclarationNode* node) override;

        std::any visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node) override;
        std::any visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node) override;

        std::any visitLiteralExpressionNode(ASTLiteralExpressionNode* node) override;
        std::any visitVariableExpressionNode(ASTVariableExpressionNode* node) override;
        std::any visitUnaryExpressionNode(ASTUnaryExpressionNode* node) override;
        std::any visitBinaryExpressionNode(ASTBinaryExpressionNode* node) override;
        std::any visitCallExpressionNode(ASTCallExpressionNode* node) override;

        std::any visitAssignmentStatementNode(ASTAssignmentStatementNode* node) override;
        std::any visitIfStatementNode(ASTIfStatementNode* node) override;
        std::any visitWhileStatementNode(ASTWhileStatementNode* node) override;
        std::any visitRepeatStatementNode(ASTRepeatStatementNode* node) override;
        std::any visitForStatementNode(ASTForStatementNode* node) override;
        std::any visitCaseStatementNode(ASTCaseStatementNode* node) override;

        std::any visitCallStatementNode(ASTCallStatementNode* node) override;
};