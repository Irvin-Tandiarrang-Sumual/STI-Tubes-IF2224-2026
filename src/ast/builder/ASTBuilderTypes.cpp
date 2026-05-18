#include "../ASTBuilder.hpp"

#include <algorithm>
#include <cctype>

namespace {

// Helper kecil untuk membandingkan nama type tanpa peduli huruf besar/kecil.
std::string toLowerCopy(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
		return static_cast<char>(std::tolower(ch));
	});
	return value;
}

// Mengecek apakah ident tersebut adalah primitive type bawaan bahasa.
bool isPrimitiveTypeName(const std::string& name) {
	const std::string lowered = toLowerCopy(name);
	return lowered == "integer" || lowered == "real" || lowered == "char" ||
		   lowered == "boolean" || lowered == "string";
}

}

// Dispatcher utama untuk CST <type> ke node AST type yang sesuai.
std::unique_ptr<ASTTypeNode> ASTBuilder::buildType(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	if (node->isTerminal()) {
		if (node->getToken().type == TokenType::ident) {
			return buildTypeFromIdentifier(tokenText(node), node);
		}
		return nullptr;
	}

	switch (node->getNonTerminal()) {
		case NonTerminal::TYPE: {
			const CSTNodes* child = node->childAt(0);
			if (child == nullptr) {
				return nullptr;
			}
			if (child->isTerminal() && child->getToken().type == TokenType::ident) {
				return buildTypeFromIdentifier(tokenText(child), child);
			}
			if (!child->isTerminal()) {
				return buildType(child);
			}
			return nullptr;
		}

		case NonTerminal::ARRAY_TYPE:
			return buildArrayType(node);

		case NonTerminal::RANGE:
			return buildRangeType(node);

		case NonTerminal::ENUMERATED:
			return buildEnumeratedType(node);

		case NonTerminal::RECORD_TYPE:
			return buildRecordType(node);

		default:
			return nullptr;
	}
}

// Mengubah ident type menjadi primitive type atau named type.
std::unique_ptr<ASTTypeNode> ASTBuilder::buildTypeFromIdentifier(const std::string& name, const CSTNodes* source) {
	std::unique_ptr<ASTTypeNode> result;

	if (isPrimitiveTypeName(name)) {
		result = std::make_unique<ASTPrimitiveType>(toLowerCopy(name));
	} else {
		result = std::make_unique<ASTNamedTypeNode>(name);
	}

	if (result != nullptr && source != nullptr) {
		result->location_ = source->getLocation();
		result->isAnonymous = isPrimitiveTypeName(name);
	}

	return result;
}

// Mengubah CST <array-type> menjadi ASTArrayTypeNode.
std::unique_ptr<ASTArrayTypeNode> ASTBuilder::buildArrayType(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	const CSTNodes* indexNode = node->childAt(2);
	const CSTNodes* elementNode = node->childAt(5);
	if (indexNode == nullptr || elementNode == nullptr) {
		return nullptr;
	}

	std::unique_ptr<ASTTypeNode> indexType;
	if (!indexNode->isTerminal() && indexNode->getNonTerminal() == NonTerminal::RANGE) {
		indexType = buildRangeType(indexNode);
	} else if (indexNode->isTerminal() && indexNode->getToken().type == TokenType::ident) {
		indexType = buildTypeFromIdentifier(tokenText(indexNode), indexNode);
	} else {
		indexType = buildType(indexNode);
	}

	std::unique_ptr<ASTTypeNode> elementType = buildType(elementNode);
	auto result = std::make_unique<ASTArrayTypeNode>(std::move(indexType), std::move(elementType));
	result->location_ = node->getLocation();
	result->isAnonymous = true;
	return result;
}

// Mengubah CST <range> menjadi ASTRangeType berisi batas awal dan akhir.
std::unique_ptr<ASTRangeType> ASTBuilder::buildRangeType(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	const CSTNodes* startNode = node->childAt(0);
	const CSTNodes* endNode = node->childAt(3);
	if (startNode == nullptr || endNode == nullptr) {
		return nullptr;
	}

	auto startConstant = buildConstant(startNode);
	auto endConstant = buildConstant(endNode);
	auto result = std::make_unique<ASTRangeType>(std::move(startConstant), std::move(endConstant));
	result->location_ = node->getLocation();
	result->isAnonymous = true;
	return result;
}

// Mengubah CST <enumerated> menjadi daftar enumerator AST.
std::unique_ptr<ASTEnumeratedTypeNode> ASTBuilder::buildEnumeratedType(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	std::vector<std::string> elements;
	const auto& children = node->getChildren();
	for (std::size_t index = 1; index + 1 < children.size(); index += 2) {
		const CSTNodes* child = children[index];
		if (child != nullptr && child->isTerminal() && child->getToken().type == TokenType::ident) {
			elements.push_back(tokenText(child));
		}
	}

	auto result = std::make_unique<ASTEnumeratedTypeNode>(std::move(elements));
	result->location_ = node->getLocation();
	result->isAnonymous = true;
	return result;
}

// Mengubah CST <record-type> menjadi ASTRecordTypeNode berisi field list.
std::unique_ptr<ASTRecordTypeNode> ASTBuilder::buildRecordType(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return nullptr;
	}

	const CSTNodes* fieldListNode = node->childAt(1);
	std::vector<ASTRecordFieldNode> fields = buildFieldList(fieldListNode);
	auto result = std::make_unique<ASTRecordTypeNode>(std::move(fields));
	result->location_ = node->getLocation();
	result->isAnonymous = true;
	return result;
}

// Mengubah CST <field-list> menjadi vector field record AST.
std::vector<ASTRecordFieldNode> ASTBuilder::buildFieldList(const CSTNodes* node) {
	std::vector<ASTRecordFieldNode> fields;
	if (node == nullptr || node->isError()) {
		return fields;
	}

	const auto& children = node->getChildren();
	for (std::size_t index = 0; index < children.size(); index += 2) {
		const CSTNodes* child = children[index];
		if (child != nullptr) {
			fields.push_back(buildFieldPart(child));
		}
	}

	return fields;
}

// Mengubah CST <field-part> seperti "x, y: integer" menjadi satu field record AST.
ASTRecordFieldNode ASTBuilder::buildFieldPart(const CSTNodes* node) {
	if (node == nullptr || node->isError()) {
		return ASTRecordFieldNode({}, nullptr);
	}

	const CSTNodes* identifierListNode = node->childAt(0);
	const CSTNodes* typeNode = node->childAt(2);
	std::vector<std::string> identifiers = buildIdentifierList(identifierListNode);
	std::unique_ptr<ASTTypeNode> fieldType = buildType(typeNode);
	return ASTRecordFieldNode(std::move(identifiers), std::move(fieldType));
}

// Mengambil semua ident dari CST <identifier-list> dan membuang token koma.
std::vector<std::string> ASTBuilder::buildIdentifierList(const CSTNodes* node) {
	std::vector<std::string> identifiers;
	if (node == nullptr || node->isError()) {
		return identifiers;
	}

	const auto& children = node->getChildren();
	for (const CSTNodes* child : children) {
		if (child != nullptr && child->isTerminal() && child->getToken().type == TokenType::ident) {
			identifiers.push_back(tokenText(child));
		}
	}

	return identifiers;
}
