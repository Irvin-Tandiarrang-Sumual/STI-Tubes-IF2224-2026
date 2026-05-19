#include "../ASTBuilder.hpp"

// Production Rule : <const-declaration> -> constsy + (ident + eql + <constant> + semicolon)+
// Semantic Rule : const-declaration = new ASTConstDeclarationNode(name, value)
void ASTBuilder::buildConstDeclaration(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out) {
    if (node == nullptr) {
        return;
    }

    // Iterasi melalui anak-anak: ident + eql + constant + semicolon pattern
    const auto& children = node->getChildren();

    int i = 0;
    // Skip first child (constsy token)
    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::constsy) {
        i++;
    }

    // Proses tiap deklarasi
    while (i < children.size()) {
        if (i >= children.size()) break;
        const CSTNodes* identNode = children[i];
        if (identNode->isTerminal() && identNode->getToken().type == TokenType::ident) {
            std::string name = tokenText(identNode);
            i++;

            if (i < children.size() && children[i]->isTerminal() &&
                children[i]->getToken().type == TokenType::eql) {
                i++;

                if (i < children.size()) {
                    const CSTNodes* constantNode = children[i];
                    auto value = buildConstant(constantNode);
                    if (value != nullptr) {
                        out.push_back(new ASTConstDeclarationNode(name, std::move(value)));
                    }
                    i++;
                }
            }
        }

        // Skip semicolon ke deklarasi selanjutnya
        if (i < children.size() && children[i]->isTerminal() &&
            children[i]->getToken().type == TokenType::semicolon) {
            i++;
        }
    }
}

// Production Rule : <type-declaration> -> typesy + (ident + eql + <type> + semicolon)+
// Semantic Rule : type-declaration = new ASTTypeDeclarationNode(name, typeDefinition)
void ASTBuilder::buildTypeDeclaration(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out) {
    if (node == nullptr) {
        return;
    }

    const auto& children = node->getChildren();
    int i = 0;

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::typesy) {
        i++;
    }

    while (i < children.size()) {
        if (i >= children.size()) break;
        const CSTNodes* identNode = children[i];
        if (identNode->isTerminal() && identNode->getToken().type == TokenType::ident) {
            std::string name = tokenText(identNode);
            i++;

            if (i < children.size() && children[i]->isTerminal() &&
                children[i]->getToken().type == TokenType::eql) {
                i++;

                if (i < children.size()) {
                    const CSTNodes* typeNode = children[i];
                    auto typeDefinition = buildType(typeNode);
                    if (typeDefinition != nullptr) {
                        out.push_back(new ASTTypeDeclarationNode(name, std::move(typeDefinition)));
                    }
                    i++;
                }
            }
        }

        if (i < children.size() && children[i]->isTerminal() &&
            children[i]->getToken().type == TokenType::semicolon) {
            i++;
        }
    }
}

// Production Rule : <var-declaration> -> varsy + (<identifier-list> + colon + <type> + semicolon)+
// Semantic Rule : var-declaration = new ASTVarDeclarationNode(identifiers, type)
void ASTBuilder::buildVarDeclaration(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out) {
    if (node == nullptr) {
        return;
    }

    const auto& children = node->getChildren();
    int i = 0;

    // Skip first child (varsy token)
    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::varsy) {
        i++;
    }

    // Proses tiap deklarasi variabel
    while (i < children.size()) {
        // Build identifier list
        if (i < children.size() && !children[i]->isTerminal() &&
            children[i]->getNonTerminal() == NonTerminal::IDENTIFIER_LIST) {
            auto identifiers = buildIdentifierList(children[i]);
            i++;

            if (i < children.size() && children[i]->isTerminal() &&
                children[i]->getToken().type == TokenType::colon) {
                i++;

                if (i < children.size()) {
                    const CSTNodes* typeNode = children[i];
                    if (typeNode != nullptr && !identifiers.empty()) {
                    for (const std::string& identifier : identifiers) {
                        ASTTypeNode* singleType = buildType(typeNode);

                        if (singleType != nullptr) {
                            out.push_back(new ASTVarDeclarationNode(identifier, singleType));
                        }
                    }
                }
                    i++;
                }
            }
        }

        if (i < children.size() && children[i]->isTerminal() &&
            children[i]->getToken().type == TokenType::semicolon) {
            i++;
        }
    }
}

// Production Rule : <subprogram-declaration> -> <procedure-declaration> | <function-declaration>
// Semantic Rule : Cari berdasarkan non-terminal
ASTDeclarationNode* ASTBuilder::buildSubprogramDeclaration(const CSTNodes* node) {
    if (node == nullptr) {
        return nullptr;
    }

    const auto& children = node->getChildren();

    for (const CSTNodes* child : children) {
        if (!child->isTerminal()) {
            NonTerminal nt = child->getNonTerminal();
            if (nt == NonTerminal::PROCEDURE_DECLARATION) {
                return buildProcedureDeclaration(child);
            } else if (nt == NonTerminal::FUNCTION_DECLARATION) {
                return buildFunctionDeclaration(child);
            }
        }
    }

    return nullptr;
}

// Production Rule : <procedure-declaration> -> proceduresy + ident + (<formal-parameter-list>)? + semicolon + <block> + semicolon
// Semantic Rule : procedure-declaration = new ASTProcedureDeclarationNode(name, parameters, localDeclarations, body)
ASTDeclarationNode* ASTBuilder::buildProcedureDeclaration(const CSTNodes* node) {
    if (node == nullptr) {
        return nullptr;
    }

    const auto& children = node->getChildren();
    int i = 0;

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::proceduresy) {
        i++;
    }

    std::string name;
    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::ident) {
        name = tokenText(children[i]);
        i++;
    }

    std::vector<ASTParameterGroup> parameters;
    if (i < children.size() && !children[i]->isTerminal() &&
        children[i]->getNonTerminal() == NonTerminal::FORMAL_PARAMETER_LIST) {
        parameters = buildFormalParameterList(children[i]);
        i++;
    }

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::semicolon) {
        i++;
    }

    std::vector<ASTDeclarationNode*> localDeclarations;
    ASTBlockStatementNode* body;

    if (i < children.size() && !children[i]->isTerminal() &&
        children[i]->getNonTerminal() == NonTerminal::BLOCK) {
        buildBlock(children[i], localDeclarations, body);
    }

    // Skip trailing semicolon after block
    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::semicolon) {
        i++;
    }

    return new ASTProcedureDeclarationNode(
        name,
        std::move(parameters),
        std::move(localDeclarations),
        std::move(body)
    );
}

// Production Rule : <function-declaration> -> functionsy + ident + (<formal-parameter-list>)? + colon + ident + semicolon + <block> + semicolon
// Semantic Rule : function-declaration = new ASTFunctionDeclarationNode(name, parameters, returnTypeName, localDeclarations, body)
ASTDeclarationNode* ASTBuilder::buildFunctionDeclaration(const CSTNodes* node) {
    if (node == nullptr) {
        return nullptr;
    }

    const auto& children = node->getChildren();
    int i = 0;

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::functionsy) {
        i++;
    }

    std::string name;
    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::ident) {
        name = tokenText(children[i]);
        i++;
    }

    std::vector<ASTParameterGroup> parameters;
    if (i < children.size() && !children[i]->isTerminal() &&
        children[i]->getNonTerminal() == NonTerminal::FORMAL_PARAMETER_LIST) {
        parameters = buildFormalParameterList(children[i]);
        i++;
    }

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::colon) {
        i++;
    }

    std::string returnTypeName;
    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::ident) {
        returnTypeName = tokenText(children[i]);
        i++;
    }

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::semicolon) {
        i++;
    }

    std::vector<ASTDeclarationNode*> localDeclarations;
    ASTBlockStatementNode* body;

    if (i < children.size() && !children[i]->isTerminal() &&
        children[i]->getNonTerminal() == NonTerminal::BLOCK) {
        buildBlock(children[i], localDeclarations, body);
    }

    if (i < children.size() && children[i]->isTerminal() &&
        children[i]->getToken().type == TokenType::semicolon) {
        i++;
    }

    return new ASTFunctionDeclarationNode(
        name,
        std::move(parameters),
        returnTypeName,
        std::move(localDeclarations),
        std::move(body)
    );
}