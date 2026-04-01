#include "Lexer.hpp"

Lexer::Lexer(std::filesystem::path p)
    : path(p) {
        std::cout << "I'm trying to construct lexer\n";
    
        advance(); // reading first token
        std::cout << "I'm successfuly reading the first token\n";
    }

    
Lexer::~Lexer() {}
// nnt perlu di-adjust lg error messagenya
void Lexer::addErrors() {
    errors_.push_back("Line " + std::to_string(getCodeLocation().line) + " Column " + std::to_string(getCodeLocation().col) + " " + std::get<std::string>(currentToken.value) + " is not valid\n");
}

std::vector<std::string> Lexer::getErrors() {
    return errors_;
}

void Lexer::addTokens() {
    tokens_.push_back(currentToken);
}

std::vector<Token> Lexer::getTokens() {
    return tokens_;
}

bool Lexer::isEOF() const {
    return reader.isEOF();
}

void Lexer::tokenize() {
    while (currentToken.type != TokenType::eof) {
        addTokens();
        advance();
    }
}

CodeLocation Lexer::getCodeLocation() const {
    return reader.getLocation();
}

void Lexer::skippingWhiteSpaces() {
    while (!reader.isEOF() && isspace(reader.getCurrentCharacter())) {
        reader.advance();
    }
}

void Lexer::skippingComments() {
    int commentDepth = 1;
    // inside comment
    reader.advance();
    while (!reader.isEOF() && commentDepth > 0) {
        if (reader.getCurrentCharacter() == '{') {
            commentDepth++;
        } else if(reader.getCurrentCharacter() == '}') {
            commentDepth--;
        }
        // advance terus
        reader.advance();
    }
    // bakal udh character tepat setelah '}' ??? :/
}

void Lexer::advance() {
    while (true) {
        skippingWhiteSpaces();
        // comment yg dimulai dari ( dilakukan terpisah sajah
        if (reader.getCurrentCharacter() == '{') {
            skippingComments();
        } else {
            break;
        }
    }
    processToken();
}

void Lexer::processToken() {
    CodeLocation codeLoc = reader.getLocation();

    if (reader.isEOF()) {
        currentToken = Token(eof, "EOF", codeLoc);
        return;
    }

    char currentChar = reader.getCurrentCharacter();
    currentToken = Token(invalid_token, "Invalid Token", codeLoc);
    // process number
    if (isdigit(currentChar)) {
        processNumber();
        return;
    }

    // keyword or identifier processing
    if (isalpha(currentChar)) {
        processKeywordOrIdentifier();
        return;
    }

    // process string
    if (currentChar == '\'') {
        processStringOrCharacter();
        return;
    }

    switch (currentChar) {
        case '+':
            currentToken = Token(TokenType::plus, "+", codeLoc);
            reader.advance();
            break;
        case '-':
            currentToken = Token(TokenType::minus, "-", codeLoc);
            reader.advance();
            break;
        case '*':
            currentToken = Token(TokenType::times, "*", codeLoc);
            reader.advance();
            break;
        case '/':
            currentToken = Token(TokenType::rdiv, "/", codeLoc);
            reader.advance();
            break;
        case '=': {
            // maju 1
            reader.advance();
            if (reader.getCurrentCharacter() == '=') {
                currentToken = Token(TokenType::eql, "==", codeLoc);
                reader.advance();
            }
            break;
        }
        case '<': {
            // maju 1
            reader.advance();
            if (reader.getCurrentCharacter() == '>') {
                currentToken = Token(TokenType::neq, "<>", codeLoc);
                reader.advance();
            } else if (reader.getCurrentCharacter() == '=') {
                currentToken = Token(TokenType::leq, "<=", codeLoc);
                reader.advance();
            } else {
                currentToken = Token(TokenType::lss, "<", codeLoc);
            }
            break;
        }
        case '>': {
            // maju 1
            reader.advance();
            if (reader.getCurrentCharacter() == '=') {
                currentToken = Token(TokenType::geq, ">=", codeLoc);
                reader.advance();
            } else {
                currentToken = Token(TokenType::gtr, ">", codeLoc);
            }
            break;
        }
        case '(': {
            // kasus perlu skip kalo ternyata comment
            // inside comment probably
            reader.advance();
            if (!reader.isEOF() && reader.getCurrentCharacter() == '*') {
                // process comment
                int commentDepth = 1;

                // inside comment
                reader.advance();
                while (!reader.isEOF() && commentDepth > 0) {
                    if (reader.getCurrentCharacter() == '*') {
                        // possible untuk otw nutup komen
                        reader.advance();

                        if (reader.getCurrentCharacter() == ')') {
                            commentDepth--;
                        }
                    } else if (reader.getCurrentCharacter() == '(') {
                        // possible nambah depth komen
                        reader.advance();
                        if (reader.getCurrentCharacter() == '*') {
                            commentDepth++;
                        }
                    }
                    reader.advance();
                }
                advance(); // token berikutnya lgsg
                return;
            } else {
                currentToken = Token(TokenType::lparent, "(", codeLoc);
                return;
            }
        }
        case ')':
            currentToken = Token(TokenType::rparent, ")", codeLoc);
            reader.advance();
            break;
        case '[':
            currentToken = Token(TokenType::lbrack, "[", codeLoc);
            reader.advance();
            break;
        case ']':
            currentToken = Token(TokenType::rbrack, "]", codeLoc);
            reader.advance();
            break;
        case ',':
            currentToken = Token(TokenType::comma, ",", codeLoc);
            reader.advance();
            break;
        case ';':
            currentToken = Token(TokenType::semicolon, ";", codeLoc);
            reader.advance();
            break;
        case '.':
            currentToken = Token(TokenType::period, ".", codeLoc);
            reader.advance();
            break;
        case ':': {
            reader.advance();
            if (reader.getCurrentCharacter() == '=') {
                currentToken = Token(TokenType::becomes, ":=", codeLoc);
                reader.advance();
            } else {
                currentToken = Token(TokenType::colon, ":", codeLoc);
            }
            break;
        }
        default:
            // ga perlu ngubah apa" lg, sisa tambahin error message(?)
            addErrors();
            currentToken = Token(TokenType::invalid_token, reader.getCurrentCharacter(), codeLoc);
            reader.advance();
            break;
        }
}

void Lexer::processStringOrCharacter() {
    std::stringstream stringOrCharStream;
    const CodeLocation codeLoc = reader.getLocation();

    stringOrCharStream << '\'';
    reader.advance(); // biar masuk lgsg ke isinya si

    while(!reader.isEOF()) {
        stringOrCharStream << reader.getCurrentCharacter();
        if (reader.getCurrentCharacter() == '\'') {
            // cek next characternya masih atau engga
            reader.advance();
            if (!reader.isEOF() && reader.getCurrentCharacter() == '\'') {
                stringOrCharStream << '\'';
                reader.advance(); // msh ragu
                continue;
            } else {
                break;
            }
        }

        reader.advance();
    }
    // prep token
    std::string content = stringOrCharStream.str();
    int isiStringDoang = content.length() - 2;
    currentToken = Token(isiStringDoang == 1?TokenType::charcon : TokenType::string, content, codeLoc);
}

void Lexer::processKeywordOrIdentifier() {
    std::stringstream keywordOrIdentifierStream;
    std::stringstream lowerStream; // lower version
    const CodeLocation codeLoc = reader.getLocation();

    // ga ada keyword yg jadi prefix keyword lain jd make ini hrsnya aman
    TokenType potentialTokenType = TokenType::ident;

    while (!reader.isEOF() && isalnum(reader.getCurrentCharacter())) {
        if (isalpha(reader.getCurrentCharacter())) {
            lowerStream << static_cast<char>(
                std::tolower(static_cast<unsigned char>(reader.getCurrentCharacter()))
            );
        } else {
            lowerStream << reader.getCurrentCharacter(); // kasus num
        }
        keywordOrIdentifierStream << reader.getCurrentCharacter();
        reader.advance();
    }
    

    // looking
    std::string value = lowerStream.str();

    if (value == "not") {
        potentialTokenType = TokenType::notsy;
    } else if (value == "div") {
        potentialTokenType = TokenType::idiv;
    } else if (value == "mod") {
        potentialTokenType = TokenType::imod;
    } else if (value == "and") {
        potentialTokenType = TokenType::andsy;
    } else if (value == "or") {
        potentialTokenType = TokenType::orsy;
    } else if (value == "const") {
        potentialTokenType = TokenType::constsy;
    } else if (value == "type") {
        potentialTokenType = TokenType::typesy;
    } else if (value == "var") {
        potentialTokenType = TokenType::varsy;
    } else if (value == "function") {
        potentialTokenType = TokenType::functionsy;
    } else if (value == "procedure") {
        potentialTokenType = TokenType::proceduresy;
    } else if (value == "array") {
        potentialTokenType = TokenType::arraysy;
    } else if (value == "record") {
        potentialTokenType = TokenType::recordsy;
    } else if (value == "program") {
        potentialTokenType = TokenType::programsy;
    } else if (value == "begin") {
        potentialTokenType = TokenType::beginsy;
    } else if (value == "if") {
        potentialTokenType = TokenType::ifsy;
    } else if (value == "case") {
        potentialTokenType = TokenType::casesy;
    } else if (value == "repeat") {
        potentialTokenType = TokenType::repeatsy;
    } else if (value == "while") {
        potentialTokenType = TokenType::whilesy;
    } else if (value == "for") {
        potentialTokenType = TokenType::forsy;
    } else if (value == "end") {
        potentialTokenType = TokenType::endsy;
    } else if (value == "else") {
        potentialTokenType = TokenType::elsesy;
    } else if (value == "until") {
        potentialTokenType = TokenType::untilsy;
    } else if (value == "of") {
        potentialTokenType = TokenType::ofsy;
    } else if (value == "do") {
        potentialTokenType = TokenType::dosy;
    } else if (value == "to") {
        potentialTokenType = TokenType::tosy;
    } else if (value == "downto") {
        potentialTokenType = TokenType::downtosy;
    } else if (value == "then") {
        potentialTokenType = TokenType::thensy;
    }
    currentToken = Token(potentialTokenType, keywordOrIdentifierStream.str(), codeLoc);
}

void Lexer::processNumber() {
    // masalah saat ini, 3. masih bakal dibaca sebagai (di python gitu" si valid ttp)
    std::stringstream numberStream;

    // read int or double value
    const CodeLocation codeLoc = reader.getLocation();
    bool haveDot = false;

    while(!reader.isEOF() && (isdigit(reader.getCurrentCharacter()) || (reader.getCurrentCharacter() == '.' && !haveDot))) {
        numberStream << reader.getCurrentCharacter();
        if (reader.getCurrentCharacter() == '.') {
            haveDot = true;
        }
        reader.advance();

    }

    if (haveDot) { // double
        double value;
        numberStream >> value;
        currentToken = Token(TokenType::realcon, value, codeLoc);
    } else {
        int value;
        numberStream >> value;
        currentToken = Token(TokenType::intcon, value, codeLoc);
    }
}
