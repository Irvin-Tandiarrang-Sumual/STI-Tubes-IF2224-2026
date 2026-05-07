#include "../Parser.hpp"

// IDENTIFIER-LIST : ident + (comma + ident)*
CSTNodes* Parser::parseIdentifierList() {
    CSTNodes* node = new CSTNodes(NonTerminal::IDENTIFIER_LIST, peek().codeLocation);
    node->addChild(expect(TokenType::ident));

    while (!isAtEnd() && check(TokenType::comma)) {
        node->addChild(expect(TokenType::comma));
        node->addChild(expect(TokenType::ident));
    }

    return node;
}

// VARIABLE : ident + (COMPONENT-VARIABLE)*
CSTNodes* Parser::parseVariable() {
    CSTNodes* node = new CSTNodes(NonTerminal::VARIABLE, peek().codeLocation);
    node->addChild(expect(TokenType::ident));
    while (!isAtEnd() && (check(TokenType::lbrack) || check(TokenType::period))) {
        node->addChild(parseComponentVariable());
    }
    return node;
}

// COMPONENT-VARIABLE : (lbrack + INDEX-LIST + rbrack) | (period + ident) 
CSTNodes* Parser::parseComponentVariable() {
    CSTNodes* node = new CSTNodes(NonTerminal::COMPONENT_VARIABLE, peek().codeLocation);

    if (check(TokenType::lbrack)) {
        // akses array: [<index-list>]
        node->addChild(expect(TokenType::lbrack));
        node->addChild(parseIndexList());
        node->addChild(expect(TokenType::rbrack));
        return node;
    }

    if (check(TokenType::period)) {
        // akses field record: . ident
        node->addChild(expect(TokenType::period));
        node->addChild(expect(TokenType::ident));
        return node;
    }
    std::string msg = makeErrorMessage("'[' or '.'");

    CSTNodes* errorNode = new CSTNodes(msg, peek().codeLocation);
    node->addChild(errorNode);
    addError(msg);

    synchronize({
        TokenType::semicolon,
        TokenType::endsy,
        TokenType::rbrack,
        TokenType::period
    });

    return node;
}

// INDEX-LIST : (intcon | charcon | ident) + (comma + INDEX-LIST)*
CSTNodes* Parser::parseIndexList() {
    CSTNodes* node = new CSTNodes(NonTerminal::INDEX_LIST, peek().codeLocation);

    // index bisa intcon, charcon, atau ident
    if (check(TokenType::intcon)) {
        node->addChild(expect(TokenType::intcon));
    } else if (check(TokenType::charcon)) {
        node->addChild(expect(TokenType::charcon));
    } else if (check(TokenType::ident)) {
        node->addChild(expect(TokenType::ident));
    } else {
        std::string msg = makeErrorMessage("intcon, charcon, or ident");

        CSTNodes* errorNode = new CSTNodes(msg, peek().codeLocation);
        node->addChild(errorNode);
        addError(msg);

        synchronize({
            TokenType::comma,
            TokenType::rbrack,
            TokenType::semicolon,
            TokenType::endsy
        });

        return node;
    }

    // kalo ada koma, berarti masih ada index lagi
    if (!isAtEnd() && check(TokenType::comma)) {
        node->addChild(expect(TokenType::comma));
        node->addChild(parseIndexList());
    }
    return node;
}

// cek apakah posisi saat ini = assignment (di pake habis cek ident di parseVariable)
bool Parser::isAssignmentStart() const {
    if (isAtEnd() || !check(TokenType::ident)) {
        return false;
    }

    size_t pos = currentPosition_ + 1;

    while (pos < tokens_.size()) {
        TokenType type = tokens_[pos].type;

        // Ketemu becomes -> ini assignment
        if (type == TokenType::becomes) {
            return true;
        }

        // lewati akses array [ ... ]
        if (type == TokenType::lbrack) {
            pos++;
            int depth = 1;
            while (pos < tokens_.size() && depth > 0) {
                if (tokens_[pos].type == TokenType::lbrack)      depth++;
                else if (tokens_[pos].type == TokenType::rbrack) depth--;
                pos++;
            }
            continue;
        }

        // lewati field access . ident
        if (type == TokenType::period) {
            pos++;
            if (pos < tokens_.size() && tokens_[pos].type == TokenType::ident) {
                pos++;
                continue;
            }
            return false; // period tanpa ident -> bukan assignment
        }
        // token lain -> bukan assignment
        return false;
    }
    return false;
}