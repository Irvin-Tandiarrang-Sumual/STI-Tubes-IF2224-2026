#include "../ASTBuilder.hpp"

std::unique_ptr<ASTBlockStatementNode> ASTBuilder::buildCompoundStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	// Struktur: beginsy + <statement-list> + endsy
	const CSTNodes* stmtListNode = node->firstChildOf(NonTerminal::STATEMENT_LIST);
	auto stmts = buildStatementList(stmtListNode);
	return std::make_unique<ASTBlockStatementNode>(std::move(stmts));
}

std::vector<std::unique_ptr<ASTStatementNode>> ASTBuilder::buildStatementList(const CSTNodes* node) {
	std::vector<std::unique_ptr<ASTStatementNode>> out;
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

std::unique_ptr<ASTStatementNode> ASTBuilder::buildStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	// Empty statement -> ASTEmptyStatementNode
	if (children.empty()) {
		return std::make_unique<ASTEmptyStatementNode>();
	}

	const CSTNodes* inner = children[0];
	if (inner == nullptr) {
		return std::make_unique<ASTEmptyStatementNode>();
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
				return std::make_unique<ASTCallStatementNode>(std::move(callExpr));
			}
		}
	}

	return std::make_unique<ASTEmptyStatementNode>();
}

std::unique_ptr<ASTAssignmentStatementNode> ASTBuilder::buildAssignmentStatement(const CSTNodes* node) {
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
	return std::make_unique<ASTAssignmentStatementNode>(std::move(target), std::move(value));
}

std::unique_ptr<ASTStatementNode> ASTBuilder::buildIfStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 4) return nullptr;

	auto condition = buildExpression(children[1]);
	auto thenBranch = buildStatement(children[3]);

	std::unique_ptr<ASTStatementNode> elseBranch = nullptr;
	if (children.size() >= 6) {
		// children[4] = elsesy token, children[5] = statement
		elseBranch = buildStatement(children[5]);
	}

	return std::make_unique<ASTIfStatementNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<ASTCaseStatementNode> ASTBuilder::buildCaseStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 5) return nullptr;

	auto condition = buildExpression(children[1]);
	std::vector<ASTCaseBranchNode> branches;
	collectCaseBranches(children[3], branches);
	return std::make_unique<ASTCaseStatementNode>(std::move(condition), std::move(branches));
}

void ASTBuilder::collectCaseBranches(const CSTNodes* node, std::vector<ASTCaseBranchNode>& out) {
	if (node == nullptr || node->isError()) {
		return;
	}

	const auto& children = node->getChildren();

	// collect leading constants (may be multiple separated by comma)
	std::vector<std::unique_ptr<ASTExpressionNode>> constants;
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
	std::unique_ptr<ASTStatementNode> body = nullptr;
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

std::unique_ptr<ASTWhileStatementNode> ASTBuilder::buildWhileStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 5) {
		return nullptr;
	}

	auto condition = buildExpression(children[1]);
	auto body = buildCompoundStatement(children[3]);
	return std::make_unique<ASTWhileStatementNode>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTRepeatStatementNode> ASTBuilder::buildRepeatStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) return nullptr;
	const auto& children = node->getChildren();
	if (children.size() < 4) return nullptr;

	auto body = buildStatementList(children[1]);
	auto condition = buildExpression(children[3]);
	return std::make_unique<ASTRepeatStatementNode>(std::move(body), std::move(condition));
}

std::unique_ptr<ASTForStatementNode> ASTBuilder::buildForStatement(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}
	const auto& children = node->getChildren();
	if (children.size() < 9) {
		return nullptr;
	}

	ASTBuilder builder;
	std::string iteratorName = builder.tokenText(children[1]);
	auto startVal = builder.buildExpression(children[3]);
	bool isDownTo = (children[4]->isTerminal() && children[4]->getToken().type == TokenType::downtosy);
	auto endVal = builder.buildExpression(children[5]);
	auto body = buildCompoundStatement(children[7]);

	return std::make_unique<ASTForStatementNode>(iteratorName, std::move(startVal), std::move(endVal), isDownTo, std::move(body));
}

std::unique_ptr<ASTCallExpressionNode> ASTBuilder::buildProcedureOrFunctionCall(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	const auto& children = node->getChildren();
	if (children.empty()) {
		return nullptr;
	}

	ASTBuilder builder;
	std::string callee = builder.tokenText(children[0]);
	std::vector<std::unique_ptr<ASTExpressionNode>> args;
	// parameter list may be at index 2 (between lparent and rparent)
	for (const CSTNodes* ch : children) {
		if (ch == nullptr) continue;
		if (!ch->isTerminal() && ch->getNonTerminal() == NonTerminal::PARAMETER_LIST) {
			args = builder.buildParameterList(ch);
			break;
		}
	}

	return std::make_unique<ASTCallExpressionNode>(callee, std::move(args));
}