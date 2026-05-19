#include "../ASTBuilder.hpp"

// Production Rule : <program> -> <program-header> + <declaration-part> + <compound-statement> + period
// Semantic Rule : program = new ASTProgramNode(programName, declarations, mainBlock)
ASTProgramNode* ASTBuilder::buildProgram(const CSTNodes* node){
    // Ekstrak nama program
    std::string programName = buildProgramHeader(node->firstChildOf(NonTerminal::PROGRAM_HEADER));

    // Bangun bagian deklarasi
    std::vector<ASTDeclarationNode*> declarations;
    buildDeclarationPart(node->firstChildOf(NonTerminal::DECLARATION_PART), declarations);
    auto mainBlock = buildCompoundStatement(node->firstChildOf(NonTerminal::COMPOUND_STATEMENT));

    return new ASTProgramNode(
        programName,
        std::move(declarations),
        std::move(mainBlock)
    );
}

// Production Rule : <program-header> -> programsy + ident + semicolon
// Semantic Rule : programName = ident
std::string ASTBuilder::buildProgramHeader(const CSTNodes* node){
    return tokenText(node->firstTokenOf(TokenType::ident));
}

// Production Rule : <declaration-part> -> (<const-declaration> | <type-declaration> | <var-declaration> | <subprogram-declaration>)*
// Semantic Rule : declarations = [const-declaration*, type-declaration*, var-declaration*, subprogram-declaration*]
void ASTBuilder::buildDeclarationPart(const CSTNodes* node, std::vector<ASTDeclarationNode*>& out){
    if (node == nullptr) {
        return;
    }

    for (const CSTNodes* child : node->getChildren()) {
        const NonTerminal& nt = child->getNonTerminal();
        if (nt == NonTerminal::CONST_DECLARATION) {
            buildConstDeclaration(child, out);
        }
        else if (nt == NonTerminal::TYPE_DECLARATION) {
            buildTypeDeclaration(child, out);
        }
        else if (nt == NonTerminal::VAR_DECLARATION) {
            buildVarDeclaration(child, out);
        }
        else if (nt == NonTerminal::SUBPROGRAM_DECLARATION) {
            auto subprog = buildSubprogramDeclaration(child);
            if (subprog != nullptr) {
                out.push_back(std::move(subprog));
            }
        }
    }
}

// Production Rule : <block> -> <declaration-part> + <compound-statement>
// Semantic Rule : block = (localDeclarations, body)
void ASTBuilder::buildBlock(const CSTNodes* node, std::vector<ASTDeclarationNode*>& localDeclarations, ASTBlockStatementNode*& body){
    if (node == nullptr) {
        return;
    }

    buildDeclarationPart(node->childAt(0), localDeclarations);
    body = buildCompoundStatement(node->childAt(1));
}
