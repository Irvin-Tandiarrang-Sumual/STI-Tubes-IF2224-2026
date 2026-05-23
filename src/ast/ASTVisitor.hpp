#pragma once

#include <any>
class ASTNode;
class ASTTypeNode;
class ASTPrimitiveType;
class ASTNamedTypeNode;
class ASTRangeType;
class ASTArrayTypeNode;
class ASTEnumeratedTypeNode;
class ASTRecordTypeNode;

class ASTExpressionNode;
class ASTLiteralExpressionNode;
class ASTVariableExpressionNode;
class ASTUnaryExpressionNode;
class ASTBinaryExpressionNode;
class ASTCallExpressionNode;

class ASTStatementNode;
class ASTEmptyStatementNode;
class ASTBlockStatementNode;
class ASTAssignmentStatementNode;
class ASTIfStatementNode;
class ASTWhileStatementNode;
class ASTRepeatStatementNode;
class ASTForStatementNode;
class ASTCaseStatementNode;
class ASTCallStatementNode;

class ASTDeclarationNode;
class ASTDeclarationsNode;
class ASTConstDeclarationNode;
class ASTTypeDeclarationNode;
class ASTVarDeclarationNode;
class ASTSubprogramDeclarationNode;
class ASTProcedureDeclarationNode;
class ASTFunctionDeclarationNode;

class ASTProgramNode;

class ASTVisitor {
    public:
        std::any visit(ASTNode *node);
        std::any visitChildren(ASTNode *node);
        // visitor methods

    virtual std::any visitTypeNode(ASTTypeNode* node);
    virtual std::any visitPrimitiveType(ASTPrimitiveType* node);
    virtual std::any visitNamedTypeNode(ASTNamedTypeNode* node);
    virtual std::any visitRangeType(ASTRangeType* node);
    virtual std::any visitArrayTypeNode(ASTArrayTypeNode* node);
    virtual std::any visitEnumeratedTypeNode(ASTEnumeratedTypeNode* node);
    virtual std::any visitRecordTypeNode(ASTRecordTypeNode* node);

    virtual std::any visitExpressionNode(ASTExpressionNode* node);
    virtual std::any visitLiteralExpressionNode(ASTLiteralExpressionNode* node);
    virtual std::any visitVariableExpressionNode(ASTVariableExpressionNode* node);
    virtual std::any visitUnaryExpressionNode(ASTUnaryExpressionNode* node);
    virtual std::any visitBinaryExpressionNode(ASTBinaryExpressionNode* node);
    virtual std::any visitCallExpressionNode(ASTCallExpressionNode* node);

    virtual std::any visitStatementNode(ASTStatementNode* node);
    virtual std::any visitEmptyStatementNode(ASTEmptyStatementNode* node);
    virtual std::any visitBlockStatementNode(ASTBlockStatementNode* node);
    virtual std::any visitAssignmentStatementNode(ASTAssignmentStatementNode* node);
    virtual std::any visitIfStatementNode(ASTIfStatementNode* node);
    virtual std::any visitWhileStatementNode(ASTWhileStatementNode* node);
    virtual std::any visitRepeatStatementNode(ASTRepeatStatementNode* node);
    virtual std::any visitForStatementNode(ASTForStatementNode* node);
    virtual std::any visitCaseStatementNode(ASTCaseStatementNode* node);
    virtual std::any visitCallStatementNode(ASTCallStatementNode* node);

    virtual std::any visitDeclarationNode(ASTDeclarationNode* node);
    virtual std::any visitDeclarationsNode(ASTDeclarationsNode* node);
    virtual std::any visitConstDeclarationNode(ASTConstDeclarationNode* node);
    virtual std::any visitTypeDeclarationNode(ASTTypeDeclarationNode* node);
    virtual std::any visitVarDeclarationNode(ASTVarDeclarationNode* node);
    virtual std::any visitSubprogramDeclarationNode(ASTSubprogramDeclarationNode* node);
    virtual std::any visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node);
    virtual std::any visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node);

    virtual std::any visitProgramNode(ASTProgramNode* node);
};