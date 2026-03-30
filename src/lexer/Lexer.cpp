#include "Lexer.hpp"

Lexer::Lexer(std::filesystem::path p)
    : path(p) {
        std::cout << "I'm trying to construct lexer\n";
    


        advance(); // reading first token
        std::cout << "I'm successfuly reading the first token\n";
    }

Lexer::~Lexer() {}

bool Lexer::isEOF() const {
    return reader.isEOF();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isEOF()) {
        tokens.push_back(currentToken);
        advance();
        /* code */
    }
    return tokens;
    
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
    switch (reader.getCurrentCharacter()) {
        case '{': {
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
            break;
        }
        case '(': {
            // inside comment probably
            char nextChar = reader.peekNextChar();
            if (nextChar == '*') {
                reader.advance(); // baca si *

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
                // udah karakter setelah si komen 
            }
            break;
        }
        default:
            break;
    }
}

void Lexer::advance() {
    skippingWhiteSpaces();
    skippingComments();

    currentToken = processToken();

}

Token Lexer::processToken() {
    char currentChar = reader.getCurrentCharacter();
    if (reader.isEOF()) {
        return Token(eof, "EOF", reader.getLocation());
    }

    // process number
    if (isdigit(currentChar)) {
        return processNumber();
    }

    // keyword or identifier processing
    if (isalpha(currentChar)) {
        return processKeywordOrIdentifier();
    }

    // process string
    if (currentChar == '\'') {
        return processStringOrCharacter();
    }
    CodeLocation codeLoc = reader.getLocation();

    switch (currentChar) {
        case '+':
            reader.advance(); // newly added
            return Token(TokenType::plus, "+", codeLoc);
        case '-':
            reader.advance(); // newly added
            return Token(TokenType::minus, "-", codeLoc);
        case '*':
            reader.advance(); // newly added
            return Token(TokenType::times, "*", codeLoc);
        case '/':
            reader.advance(); // newly added
            return Token(TokenType::rdiv, "/", codeLoc);
        case '=': {
            char nextChar = reader.peekNextChar();
            if (nextChar == '=') {
                // formed full ==
                reader.advance();
                reader.advance(); // newly added
                return Token(TokenType::eql, "==", codeLoc);
            } else {
                reader.advance();
                return Token(TokenType::invalid_token, "=", codeLoc);
            }
        }
        case '<': {
            char nextChar = reader.peekNextChar();
            if (nextChar == '>') {
                reader.advance();
                reader.advance(); // newly added
                return Token(TokenType::neq, "<>", codeLoc);
            } else if (nextChar == '=') {
                reader.advance();
                reader.advance(); // newly added
                return Token(TokenType::leq, "<=", codeLoc);
            } else {
                reader.advance(); // newly added

                return Token(TokenType::lss, "<", codeLoc);
            }
        }
        case '>': {
            char nextChar = reader.peekNextChar();
            if (nextChar == '=') {
                reader.advance();
                reader.advance(); // newly added

                return Token(TokenType::geq, ">=", codeLoc);
            } else {
                reader.advance(); // newly added

                return Token(TokenType::gtr, ">", codeLoc);
            }
        }
        case '(':
            reader.advance(); // newly added

            return Token(TokenType::lparent, "(", codeLoc);
        case ')':
            reader.advance(); // newly added

            return Token(TokenType::rparent, ")", codeLoc);
        case '[':
            reader.advance(); // newly added

            return Token(TokenType::lbrack, "[", codeLoc);
        case ']':
            reader.advance(); // newly added

            return Token(TokenType::rbrack, "]", codeLoc);
        case ',':
            reader.advance(); // newly added

            return Token(TokenType::comma, ",", codeLoc);
        case ';':
            reader.advance(); // newly added

            return Token(TokenType::semicolon, ";", codeLoc);
        case '.':
            reader.advance(); // newly added

            return Token(TokenType::period, ".", codeLoc);
        case ':': {
            char nextChar = reader.peekNextChar();
            std::cout << "DEBUG ':' nextChar = [" << nextChar << "]\n";
            if (nextChar == '=') {
                reader.advance();
                reader.advance(); // newly added

                return Token(TokenType::becomes, ":=", codeLoc);
            } else {
                reader.advance(); // newly added

                return Token(TokenType::colon, ":", codeLoc);
            }
        }
        default:
            return Token(TokenType::invalid_token, std::string (1, currentChar), codeLoc);
        }
}

Token Lexer::processStringOrCharacter() {
    std::stringstream stringOrCharStream;
    const CodeLocation codeLoc = reader.getLocation();
    reader.advance();
    // make sure udh isinya atau ketemu si ' lagi
    while(!reader.isEOF()) {

        if (reader.getCurrentCharacter() == '\'') {
            // cek next characternya masih atau engga
            if (reader.peekNextChar() == '\'') {
               stringOrCharStream << '\'';
               reader.advance();
               reader.advance();
               continue;
            } else {
                reader.advance();
                break;
            }
        }

        stringOrCharStream << reader.getCurrentCharacter();
        reader.advance();
    }
    std::string content = stringOrCharStream.str();
    return Token(content.length() == 1?TokenType::charcon : TokenType::string, content, codeLoc);
}

Token Lexer::processKeywordOrIdentifier() {
    std::stringstream keywordOrIdentifierStream;
    std::stringstream lowerStream; // lower version
    const CodeLocation codeLoc = reader.getLocation();

    // ga ada keyword yg jadi prefix keyword lain jd make ini hrsnya aman
    TokenType potentialTokenType = TokenType::ident;

    do {
        char c = reader.getCurrentCharacter();
        if (isalpha(reader.getCurrentCharacter())){
            lowerStream << std::tolower(c);
        } else {
            lowerStream << c;
        }
        keywordOrIdentifierStream << c;

        reader.advance();
    } while (isalnum(reader.getCurrentCharacter()));

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
    return Token(potentialTokenType, keywordOrIdentifierStream.str(), codeLoc);
}

Token Lexer::processNumber() {
    std::stringstream numberStream;

    // read int or double value
    const CodeLocation codeLoc = reader.getLocation();
    bool haveDot = false;
    do {
        char c = reader.getCurrentCharacter();
        if (c == '.') {
            haveDot = true;
        }
        numberStream << c;
        reader.advance();
    } while (isdigit(reader.getCurrentCharacter()) || (reader.getCurrentCharacter() == '.' && !haveDot));

    if (haveDot) {
        double value;
        numberStream >> value;
        return Token(TokenType::realcon, value, codeLoc);
    } else {
        int value;
        numberStream >> value;
        return Token(TokenType::intcon, value, codeLoc);
    }
}
