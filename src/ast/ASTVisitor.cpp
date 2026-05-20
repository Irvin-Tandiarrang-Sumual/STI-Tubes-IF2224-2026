#include "ASTVisitor.hpp"

#include "ASTNodes.hpp"

std::any ASTVisitor::visit(ASTNode *node) {
    if (node == nullptr) {
        return std::any{};
    }

    return node->accept(this);
}

std::any ASTVisitor::visitChildren(ASTNode *node) {
	if (node == nullptr) {
		return std::any{};
	}

	std::any lastResult;
	for (ASTNode *child : node->children_) {
		std::any childResult = visit(child);
		if (childResult.has_value()) {
			lastResult = std::move(childResult);
		}
	}
	return lastResult;
}

std::any ASTVisitor::visitTypeNode(ASTTypeNode *node) {
	return visitChildren(node);
}

std::any ASTVisitor::visitPrimitiveType(ASTPrimitiveType *node) {
	return visitTypeNode(node);
}

std::any ASTVisitor::visitNamedTypeNode(ASTNamedTypeNode *node) {
	return visitTypeNode(node);
}

std::any ASTVisitor::visitRangeType(ASTRangeType *node) {
	return visitTypeNode(node);
}

std::any ASTVisitor::visitArrayTypeNode(ASTArrayTypeNode *node) {
	return visitTypeNode(node);
}

std::any ASTVisitor::visitEnumeratedTypeNode(ASTEnumeratedTypeNode *node) {
	return visitTypeNode(node);
}

std::any ASTVisitor::visitRecordTypeNode(ASTRecordTypeNode *node) {
	return visitTypeNode(node);
}

std::any ASTVisitor::visitExpressionNode(ASTExpressionNode *node) {
	return visitChildren(node);
}

std::any ASTVisitor::visitLiteralExpressionNode(ASTLiteralExpressionNode *node) {
	return visitExpressionNode(node);
}

std::any ASTVisitor::visitVariableExpressionNode(ASTVariableExpressionNode *node) {
	return visitExpressionNode(node);
}

std::any ASTVisitor::visitUnaryExpressionNode(ASTUnaryExpressionNode *node) {
	return visitExpressionNode(node);
}

std::any ASTVisitor::visitBinaryExpressionNode(ASTBinaryExpressionNode *node) {
	return visitExpressionNode(node);
}

std::any ASTVisitor::visitCallExpressionNode(ASTCallExpressionNode *node) {
	return visitExpressionNode(node);
}

std::any ASTVisitor::visitStatementNode(ASTStatementNode *node) {
	return visitChildren(node);
}

std::any ASTVisitor::visitEmptyStatementNode(ASTEmptyStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitBlockStatementNode(ASTBlockStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitAssignmentStatementNode(ASTAssignmentStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitIfStatementNode(ASTIfStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitWhileStatementNode(ASTWhileStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitRepeatStatementNode(ASTRepeatStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitForStatementNode(ASTForStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitCaseStatementNode(ASTCaseStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitCallStatementNode(ASTCallStatementNode *node) {
	return visitStatementNode(node);
}

std::any ASTVisitor::visitDeclarationNode(ASTDeclarationNode *node) {
	return visitChildren(node);
}

std::any ASTVisitor::visitDeclarationsNode(ASTDeclarationsNode *node) {
	return visitChildren(node);
}

std::any ASTVisitor::visitConstDeclarationNode(ASTConstDeclarationNode *node) {
	return visitDeclarationNode(node);
}

std::any ASTVisitor::visitTypeDeclarationNode(ASTTypeDeclarationNode *node) {
	return visitDeclarationNode(node);
}

std::any ASTVisitor::visitVarDeclarationNode(ASTVarDeclarationNode *node) {
	return visitDeclarationNode(node);
}

std::any ASTVisitor::visitSubprogramDeclarationNode(ASTSubprogramDeclarationNode *node) {
	return visitDeclarationNode(node);
}

std::any ASTVisitor::visitProcedureDeclarationNode(ASTProcedureDeclarationNode *node) {
	return visitSubprogramDeclarationNode(node);
}

std::any ASTVisitor::visitFunctionDeclarationNode(ASTFunctionDeclarationNode *node) {
	return visitSubprogramDeclarationNode(node);
}

std::any ASTVisitor::visitProgramNode(ASTProgramNode *node) {
	return visitChildren(node);
}

