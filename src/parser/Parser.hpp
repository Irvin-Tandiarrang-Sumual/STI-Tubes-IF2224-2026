#include <iostream>
#include <vector>
#include "../lexer/Token.hpp"
#include "../cst/CSTNodes.hpp"
#include "NonTerminal.hpp"
#include "SyntaxError.hpp"
class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser();
        
        // return root node aja
        CSTNodes* parse();

    private:
        std::vector<std::string> errorMessages_;
        std::vector<std::string> getErrors() const;
        std::string makeErrorMessage(const std::string& expected);
        void addError(const std::string& msg);
        const std::vector<Token>& tokens_;
        size_t currentPosition_ = 0;

        // lihat top saat ini
        const Token& peek() const;

        // peek one a head
        const Token& peekNext() const;

        // periksa apakah sesuai yang dimau dan panggil advance jika sesuai
        // kalo engga bakal throw exception
        CSTNodes* expect(TokenType token);

        void advance();

        // buat sync ketika terjadi error (maju sampe titik "aman")
        void synchronize(std::vector<TokenType> syncSet);

        // skip comment krn ga kepake
        void skipUselessToken();

        // cek token sekarang tanpa advance
        bool check(TokenType type) const;
        
        // true at least 1 match
        bool checkMultiple(const std::vector<TokenType>& tokenTypes) const;

        // cek apakah sudah habis
        bool isAtEnd() const;

        // cek apakah statement sekarang adalah assignment (lookahead)
        bool isAssignmentStart() const;

        CSTNodes* errorNode(std::string message);

        // parse based on production rules
        // program structure
        CSTNodes* parseProgram();

        CSTNodes* parseProgramHeader();

        CSTNodes* parseBlock();

        // declarations
        CSTNodes* parseDeclarationPart();

        CSTNodes* parseConstDeclaration();

        CSTNodes* parseConstant();

        CSTNodes* parseTypeDeclaration();

        CSTNodes* parseVarDeclaration();

        CSTNodes* parseSubprogramDeclaration();

        CSTNodes* parseProcedureDeclaration();

        CSTNodes* parseFunctionDeclaration();


        // types
        CSTNodes* parseType();

        CSTNodes* parseArrayType();

        CSTNodes* parseRange();

        CSTNodes* parseEnumerated();

        CSTNodes* parseRecordType();

        CSTNodes* parseFieldList();

        CSTNodes* parseFieldPart();

        // parameters

        CSTNodes* parseFormalParameterList();

        CSTNodes* parseParameterGroup();

        CSTNodes* parseParameterList();

        // statements
        CSTNodes* parseCompoundStatement();

        CSTNodes* parseStatementList();

        CSTNodes* parseStatement();

        // execution statements
        CSTNodes* parseAssignmentStatement();

        CSTNodes* parseProcedureOrFunctionCall();

        // control flow statements
        CSTNodes* parseIfStatement();

        CSTNodes* parseCaseStatement();

        CSTNodes* parseCaseBlock();

        CSTNodes* parseWhileStatement();

        CSTNodes* parseRepeatStatement();

        CSTNodes* parseForStatement();

        // variables
        CSTNodes* parseVariable();

        CSTNodes* parseComponentVariable();

        CSTNodes* parseIndexList();

        CSTNodes* parseIdentifierList();

        // expressions
        CSTNodes* parseExpression();

        CSTNodes* parseSimpleExpression();

        CSTNodes* parseTerm();

        CSTNodes* parseFactor();

        // operators
        CSTNodes* parseRelationalOperator();

        CSTNodes* parseAdditiveOperator();

        CSTNodes* parseMultiplicativeOperator();
};


