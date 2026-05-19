#include "../ASTBuilder.hpp"

ASTBlockStatementNode* ASTBuilder::buildCompoundStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	// Struktur: beginsy + <statement-list> + endsy
	const CSTNodes* stmtListNode = node->firstChildOf(NonTerminal::STATEMENT_LIST);
	auto stmts = buildStatementList(stmtListNode);
	return new ASTBlockStatementNode(std::move(stmts));
}

std::vector<ASTStatementNode*> ASTBuilder::buildStatementList(const CSTNodes* node) {
	std::vector<ASTStatementNode*> out;
	if (node == nullptr || node->isError()) {
		return out;
	}
	const auto& children = node->getChildren();
	for (const CSTNodes* child : children) {
		if (child == nullptr) continue;
		if (!child->isTerminal() && child->getNonTerminal() == NonTerminal::STATEMENT) {
			auto stmt = buildStatement(child);
			if (stmt != nullptr) out.push_back(std::move(stmt));
		}
	}

	return out;
}

ASTStatementNode* ASTBuilder::buildStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	// Empty statement -> ASTEmptyStatementNode
	if (children.empty()) {
		return new ASTEmptyStatementNode();
	}

	const CSTNodes* inner = children[0];
	if (inner == nullptr) {
		return new ASTEmptyStatementNode();
	}

	if (!inner->isTerminal()) {
		NonTerminal nt = inner->getNonTerminal();
		if (nt == NonTerminal::ASSIGNMENT_STATEMENT) {
			return buildAssignmentStatement(inner);
		} else if (nt == NonTerminal::IF_STATEMENT) {
			return buildIfStatement(inner);
		} else if (nt == NonTerminal::CASE_STATEMENT) {
			return buildCaseStatement(inner);
		} else if (nt == NonTerminal::WHILE_STATEMENT) {
			return buildWhileStatement(inner);
		} else if (nt == NonTerminal::REPEAT_STATEMENT) {
			return buildRepeatStatement(inner);
		} else if (nt == NonTerminal::FOR_STATEMENT) {
			return buildForStatement(inner);
		} else if (nt == NonTerminal::PROCEDURE_OR_FUNCTION_CALL) {
			auto callExpr = buildProcedureOrFunctionCall(inner);
			if (callExpr != nullptr) {
				return new ASTCallStatementNode(std::move(callExpr));
			}
		}
	}

	return new ASTEmptyStatementNode();
}

ASTAssignmentStatementNode* ASTBuilder::buildAssignmentStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) return nullptr;
	const auto& children = node->getChildren();
	if (children.size() < 3) {
		return nullptr;
	}
	auto target = buildVariable(children[0]);
	auto value = buildExpression(children[2]);
	if (target == nullptr || value == nullptr) {
		return nullptr;
	}
	return new ASTAssignmentStatementNode(std::move(target), std::move(value));
}

ASTStatementNode* ASTBuilder::buildIfStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 4) return nullptr;

	auto condition = buildExpression(children[1]);
	auto thenBranch = buildStatement(children[3]);

	ASTStatementNode* elseBranch = nullptr;
	if (children.size() >= 6) {
		// children[4] = elsesy token, children[5] = statement
		elseBranch = buildStatement(children[5]);
	}

	return new ASTIfStatementNode(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

ASTCaseStatementNode* ASTBuilder::buildCaseStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 5) return nullptr;

	auto condition = buildExpression(children[1]);
	std::vector<ASTCaseBranchNode> branches;
	collectCaseBranches(children[3], branches);
	return new ASTCaseStatementNode(std::move(condition), std::move(branches));
}

void ASTBuilder::collectCaseBranches(const CSTNodes* node, std::vector<ASTCaseBranchNode>& out) {
	if (node == nullptr || node->isError()) {
		return;
	}

	const auto& children = node->getChildren();

	// collect leading constants (may be multiple separated by comma)
	std::vector<ASTExpressionNode*> constants;
	std::size_t i = 0;
	while (i < children.size()) {
		const CSTNodes* ch = children[i];
		if (ch == nullptr) {
			break;
		}
		if (!ch->isTerminal() && ch->getNonTerminal() == NonTerminal::CONSTANT) {
			constants.push_back(buildConstant(ch));
			i++;
			// skip optional comma tokens between constants
			if (i < children.size() && children[i]->isTerminal() && children[i]->getToken().type == TokenType::comma) {
				i++; // skip comma and continue collecting constants
				continue;
			}
			break;
		}
		break;
	}

	// expect colon
	if (i < children.size() && children[i]->isTerminal() && children[i]->getToken().type == TokenType::colon) i++;

	// statement body
	ASTStatementNode* body = nullptr;
	if (i < children.size() && !children[i]->isTerminal() && children[i]->getNonTerminal() == NonTerminal::STATEMENT) {
		body = buildStatement(children[i]);
		i++;
	}

	out.emplace_back(std::move(constants), std::move(body));

	// There may be following parts: semicolon and nested CASE_BLOCK(s)
	for (std::size_t j = i; j < children.size(); ++j) {
		const CSTNodes* ch = children[j];
		if (ch == nullptr) {
			continue;
		}
		if (!ch->isTerminal() && ch->getNonTerminal() == NonTerminal::CASE_BLOCK) {
			collectCaseBranches(ch, out);
		}
	}
}

ASTWhileStatementNode* ASTBuilder::buildWhileStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 5) {
		return nullptr;
	}

	auto condition = buildExpression(children[1]);
	auto body = buildCompoundStatement(children[3]);
	return new ASTWhileStatementNode(std::move(condition), std::move(body));
}

ASTRepeatStatementNode* ASTBuilder::buildRepeatStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) return nullptr;
	const auto& children = node->getChildren();
	if (children.size() < 4) return nullptr;

	auto body = buildStatementList(children[1]);
	auto condition = buildExpression(children[3]);
	return new ASTRepeatStatementNode(std::move(body), std::move(condition));
}

ASTForStatementNode* ASTBuilder::buildForStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 9) {
		return nullptr;
	}

	std::string iteratorName = tokenText(children[1]);
	auto startVal = buildExpression(children[3]);
	bool isDownTo = (children[4]->isTerminal() && children[4]->getToken().type == TokenType::downtosy);
	auto endVal = buildExpression(children[5]);
	auto body = buildCompoundStatement(children[7]);

	return new ASTForStatementNode(iteratorName, std::move(startVal), std::move(endVal), isDownTo, std::move(body));
}

ASTCallExpressionNode* ASTBuilder::buildProcedureOrFunctionCall(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	const auto& children = node->getChildren();
	if (children.empty()) {
		return nullptr;
	}

	std::string callee = tokenText(children[0]);
	std::vector<ASTExpressionNode*> args;
	// parameter list may be at index 2 (between lparent and rparent)
	for (const CSTNodes* ch : children) {
		if (ch == nullptr) continue;
		if (!ch->isTerminal() && ch->getNonTerminal() == NonTerminal::PARAMETER_LIST) {
			args = buildParameterList(ch);
			break;
		}
	}

	return new ASTCallExpressionNode(callee, std::move(args));
}