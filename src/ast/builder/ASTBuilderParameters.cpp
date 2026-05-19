#include "../ASTBuilder.hpp"

// Mengubah CST <formal-parameter-list> menjadi daftar ASTParameterGroup.
std::vector<ASTParameterGroup> ASTBuilder::buildFormalParameterList(const CSTNodes* node) {
	std::vector<ASTParameterGroup> parameters;
	if (node == nullptr || node->isError()) {
		return parameters;
	}

	const auto& children = node->getChildren();
	for (std::size_t index = 1; index + 1 < children.size(); index += 2) {
		const CSTNodes* child = children[index];
		if (child != nullptr) {
			parameters.push_back(buildParameterGroup(child));
		}
	}

	return parameters;
}

// Mengubah CST <parameter-group> menjadi satu grup parameter AST.
ASTParameterGroup ASTBuilder::buildParameterGroup(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return ASTParameterGroup({}, nullptr);
	}

	const CSTNodes* identifierListNode = node->childAt(0);
	const CSTNodes* typeNode = node->childAt(2);
	std::vector<std::string> identifiers = buildIdentifierList(identifierListNode);
	ASTTypeNode* type;

	if (typeNode != nullptr) {
		type = buildType(typeNode);
	}

	return ASTParameterGroup(std::move(identifiers), std::move(type));
}

// Mengubah CST <parameter-list> pada call menjadi vector argument expression AST.
std::vector<ASTExpressionNode*> ASTBuilder::buildParameterList(const CSTNodes* node) {
	std::vector<ASTExpressionNode*> parameters;
	if (node == nullptr || node->isError()) {
		return parameters;
	}

	const auto& children = node->getChildren();
	for (std::size_t index = 0; index < children.size(); index += 2) {
		const CSTNodes* child = children[index];
		if (child != nullptr) {
			parameters.push_back(buildExpression(child));
		}
	}

	return parameters;
}
