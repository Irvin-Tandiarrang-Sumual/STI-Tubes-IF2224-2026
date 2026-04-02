#include "Lexer.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

Lexer::Lexer(const std::filesystem::path &p) : path(p), reader(p) {}
Lexer::~Lexer() = default;

void Lexer::addToken(const Token &token) {
    tokens_.push_back(token);
}

void Lexer::addError(const CodeLocation &loc, const std::string &message, const std::string &lexeme) {
    std::ostringstream oss;
    oss << "Line " << loc.line << ", Column " << loc.col << ": " << message;
    if (!lexeme.empty()) {
        oss << " -> " << lexeme;
    }
    oss << '\n';
    errors_.push_back(oss.str());
}

const std::vector<std::string> &Lexer::getErrors() const {
    return errors_;
}

const std::vector<Token> &Lexer::getTokens() const {
    return tokens_;
}

bool Lexer::isEOF() const {
    return reader.isEOF();
}

CodeLocation Lexer::getCodeLocation() const {
    return reader.getLocation();
}

void Lexer::tokenize() {
    while (!reader.isEOF()) {
        skipWhitespace();
        if (reader.isEOF()) {
            break;
        }
        processToken();
    }
}

void Lexer::skipWhitespace() {
    while (!reader.isEOF() && std::isspace(static_cast<unsigned char>(reader.getCurrentCharacter()))) {
        reader.advance();
    }
}

bool Lexer::isIdentifierBody(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

bool Lexer::isDelimiter(char ch) {
    if (ch == '\0') return true;
    if (std::isspace(static_cast<unsigned char>(ch))) return true;
    switch (ch) {
        case '+': case '-': case '*': case '/': case '=': case '<': case '>': case '(': case ')':
        case '[': case ']': case ',': case ';': case '.': case ':': case '{': case '}': case '\'':
            return true;
        default:
            return false;
    }
}

std::string Lexer::lowercase(const std::string &text) {
    std::string lowered = text;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return lowered;
}

std::string Lexer::readWhileIdentifierBody() {
    std::string lexeme;
    while (!reader.isEOF() && isIdentifierBody(reader.getCurrentCharacter())) {
        lexeme.push_back(reader.getCurrentCharacter());
        reader.advance();
    }
    return lexeme;
}

std::string Lexer::readUntilDelimiter() {
    std::string lexeme;
    while (!reader.isEOF() && !isDelimiter(reader.getCurrentCharacter())) {
        lexeme.push_back(reader.getCurrentCharacter());
        reader.advance();
    }
    return lexeme;
}

void Lexer::processToken() {
    const CodeLocation loc = reader.getLocation();
    const char ch = reader.getCurrentCharacter();
    const unsigned char uch = static_cast<unsigned char>(ch);

    if (std::isalpha(uch)) {
        processIdentifierOrKeyword();
        return;
    }

    if (std::isdigit(uch)) {
        processNumber();
        return;
    }

    if (ch == '\'') {
        processStringOrCharacter();
        return;
    }

    if (ch == '{' || (ch == '(' && reader.peek() == '*')) {
        processComment();
        return;
    }

    if (ch == '_') {
        processMalformedIdentifier();
        return;
    }

    if (ch == '.' && std::isdigit(static_cast<unsigned char>(reader.peek()))) {
        processMalformedRealStartingWithDot();
        return;
    }

    switch (ch) {
        case '+': addToken(Token(plus, "+", loc)); reader.advance(); return;
        case '-': addToken(Token(minus, "-", loc)); reader.advance(); return;
        case '*': addToken(Token(times, "*", loc)); reader.advance(); return;
        case '/': addToken(Token(rdiv, "/", loc)); reader.advance(); return;
        case '(': addToken(Token(lparent, "(", loc)); reader.advance(); return;
        case ')': addToken(Token(rparent, ")", loc)); reader.advance(); return;
        case '[': addToken(Token(lbrack, "[", loc)); reader.advance(); return;
        case ']': addToken(Token(rbrack, "]", loc)); reader.advance(); return;
        case ',': addToken(Token(comma, ",", loc)); reader.advance(); return;
        case ';': addToken(Token(semicolon, ";", loc)); reader.advance(); return;
        case ':':
            if (reader.peek() == '=') {
                addToken(Token(becomes, ":=", loc));
                reader.advance();
                reader.advance();
            } else {
                addToken(Token(colon, ":", loc));
                reader.advance();
            }
            return;
        case '.':
            addToken(Token(period, ".", loc));
            reader.advance();
            return;
        case '<':
            if (reader.peek() == '=') {
                addToken(Token(leq, "<=", loc));
                reader.advance(); reader.advance();
            } else if (reader.peek() == '>') {
                addToken(Token(neq, "<>", loc));
                reader.advance(); reader.advance();
            } else {
                addToken(Token(lss, "<", loc));
                reader.advance();
            }
            return;
        case '>':
            if (reader.peek() == '=') {
                addToken(Token(geq, ">=", loc));
                reader.advance(); reader.advance();
            } else {
                addToken(Token(gtr, ">", loc));
                reader.advance();
            }
            return;
        case '=':
            if (reader.peek() == '=') {
                addToken(Token(eql, "==", loc));
                reader.advance(); reader.advance();
            } else {
                processSingleEqualsError();
            }
            return;
        default:
            processUnknownCharacter();
            return;
    }
}

void Lexer::processIdentifierOrKeyword() {
    static const std::map<std::string, TokenType> keywords = {
        {"not", notsy}, {"div", idiv}, {"mod", imod}, {"and", andsy}, {"or", orsy},
        {"const", constsy}, {"type", typesy}, {"var", varsy}, {"function", functionsy},
        {"procedure", proceduresy}, {"array", arraysy}, {"record", recordsy},
        {"program", programsy}, {"begin", beginsy}, {"if", ifsy}, {"case", casesy},
        {"repeat", repeatsy}, {"while", whilesy}, {"for", forsy}, {"end", endsy},
        {"else", elsesy}, {"until", untilsy}, {"of", ofsy}, {"do", dosy}, {"to", tosy},
        {"downto", downtosy}, {"then", thensy}
    };

    const CodeLocation loc = reader.getLocation();
    std::string lexeme;
    while (!reader.isEOF() && std::isalnum(static_cast<unsigned char>(reader.getCurrentCharacter()))) {
        lexeme.push_back(reader.getCurrentCharacter());
        reader.advance();
    }

    const std::string lowered = lowercase(lexeme);
    auto it = keywords.find(lowered);
    if (it != keywords.end()) {
        addToken(Token(it->second, lexeme, loc));
    } else {
        addToken(Token(ident, lexeme, loc));
    }
}

void Lexer::processNumber() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme;

    while (!reader.isEOF() && std::isdigit(static_cast<unsigned char>(reader.getCurrentCharacter()))) {
        lexeme.push_back(reader.getCurrentCharacter());
        reader.advance();
    }

    if (!reader.isEOF() && reader.getCurrentCharacter() == '.') {
        lexeme.push_back('.');
        reader.advance();

        if (!reader.isEOF() && std::isdigit(static_cast<unsigned char>(reader.getCurrentCharacter()))) {
            while (!reader.isEOF() && std::isdigit(static_cast<unsigned char>(reader.getCurrentCharacter()))) {
                lexeme.push_back(reader.getCurrentCharacter());
                reader.advance();
            }

            if (!reader.isEOF() && isIdentifierBody(reader.getCurrentCharacter())) {
                lexeme += readUntilDelimiter();
                addError(loc, "Format real tidak valid", lexeme);
                return;
            }

            addToken(Token(realcon, lexeme, loc));
            return;
        }

        if (!reader.isEOF() && reader.getCurrentCharacter() == '.') {
            lexeme.push_back('.');
            reader.advance();
        }
        while (!reader.isEOF() && !isDelimiter(reader.getCurrentCharacter())) {
            lexeme.push_back(reader.getCurrentCharacter());
            reader.advance();
        }
        addError(loc, "Format real tidak valid", lexeme);
        return;
    }

    if (!reader.isEOF() && isIdentifierBody(reader.getCurrentCharacter())) {
        lexeme += readUntilDelimiter();
        addError(loc, "Format angka tidak valid", lexeme);
        return;
    }

    addToken(Token(intcon, lexeme, loc));
}

void Lexer::processStringOrCharacter() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme;
    int logicalLength = 0;

    lexeme.push_back('\'');
    reader.advance();

    while (!reader.isEOF()) {
        const char ch = reader.getCurrentCharacter();
        lexeme.push_back(ch);

        if (ch == '\'') {
            if (reader.peek() == '\'') {
                lexeme.push_back('\'');
                reader.advance();
                reader.advance();
                ++logicalLength;
                continue;
            }

            reader.advance();
            addToken(Token(logicalLength == 1 ? charcon : string, lexeme, loc));
            return;
        }

        ++logicalLength;
        reader.advance();
    }

    addError(loc, "Literal string/char tidak ditutup", lexeme);
}

void Lexer::processComment() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme;
    std::vector<char> stack;

    if (reader.getCurrentCharacter() == '{') {
        lexeme.push_back('{');
        stack.push_back('{');
        reader.advance();
    } else {
        lexeme.push_back('(');
        lexeme.push_back('*');
        stack.push_back('(');
        reader.advance();
        reader.advance();
    }

    while (!reader.isEOF() && !stack.empty()) {
        const char ch = reader.getCurrentCharacter();

        if (ch == '{') {
            lexeme.push_back('{');
            stack.push_back('{');
            reader.advance();
            continue;
        }

        if (ch == '(' && reader.peek() == '*') {
            lexeme.push_back('(');
            lexeme.push_back('*');
            stack.push_back('(');
            reader.advance();
            reader.advance();
            continue;
        }

        if (ch == '}' && !stack.empty() && stack.back() == '{') {
            lexeme.push_back('}');
            stack.pop_back();
            reader.advance();
            continue;
        }

        if (ch == '*' && reader.peek() == ')' && !stack.empty() && stack.back() == '(') {
            lexeme.push_back('*');
            lexeme.push_back(')');
            stack.pop_back();
            reader.advance();
            reader.advance();
            continue;
        }

        lexeme.push_back(ch);
        reader.advance();
    }

    if (!stack.empty()) {
        addError(loc, "Comment tidak ditutup", lexeme);
        return;
    }

    addToken(Token(comment, lexeme, loc));
}

void Lexer::processUnknownCharacter() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme(1, reader.getCurrentCharacter());
    addToken(Token(invalid_token, lexeme, loc));
    reader.advance();
}

void Lexer::processMalformedIdentifier() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme = readWhileIdentifierBody();
    addError(loc, "Identifier harus diawali huruf", lexeme);
}

void Lexer::processMalformedRealStartingWithDot() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme;
    lexeme.push_back('.');
    reader.advance();
    while (!reader.isEOF() && std::isdigit(static_cast<unsigned char>(reader.getCurrentCharacter()))) {
        lexeme.push_back(reader.getCurrentCharacter());
        reader.advance();
    }
    while (!reader.isEOF() && !isDelimiter(reader.getCurrentCharacter())) {
        lexeme.push_back(reader.getCurrentCharacter());
        reader.advance();
    }
    addError(loc, "Format real tidak valid", lexeme);
}

void Lexer::processSingleEqualsError() {
    const CodeLocation loc = reader.getLocation();
    std::string lexeme(1, '=');
    reader.advance();
    addError(loc, "Operator '=' tidak valid. Gunakan '==' untuk equal", lexeme);
}
