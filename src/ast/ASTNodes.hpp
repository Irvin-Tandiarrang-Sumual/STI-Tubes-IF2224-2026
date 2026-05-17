#pragma once
#include "../reader/CodeLocation.hpp"
#include "../symboltable/DataType.hpp"
#include <vector>
#include <memory>
#include <string>
#include <variant>

class ASTNode {
    public:
        CodeLocation location_;
        std::vector<ASTNode*> children_;
        ASTNode* parent_;
        virtual ~ASTNode() = default;
};

class ASTTypeNode : public ASTNode {};
class ASTExpressionNode : public ASTNode {};
class ASTStatementNode : public ASTNode {};
class ASTDeclarationNode : public ASTNode {};


// type

// Primitive Type
// Contoh : real, integer, char, boolean, string
class ASTPrimitiveType : public ASTTypeNode {
    public:
        std::string type;
        ASTPrimitiveType(std::string type) : type(type) {}
};

// Range
// Production Rule : <range> -> <constant> + period + period + <constant>
// Semantic Rule : range = new ASTRangeType(startConstant, endConstant)
class ASTRangeType : public ASTTypeNode {
    public:
        std::unique_ptr<ASTExpressionNode> startConstant;
        std::unique_ptr<ASTExpressionNode> endConstant;
        ASTRangeType(std::unique_ptr<ASTExpressionNode> startConstant, std::unique_ptr<ASTExpressionNode> endConstant)
            : startConstant(std::move(startConstant)), endConstant(std::move(endConstant)) {}
};

// Array
// Production Rule : <array-type> -> arraysy + lbrack + (<range> | ident) + rbrack + ofsy + <type>
// Semantic Rule : array-type = new ASTArrayTypeNode(indexType, elementType)
class ASTArrayTypeNode : public ASTTypeNode {
    public:
        std::unique_ptr<ASTTypeNode> indexType;
        std::unique_ptr<ASTTypeNode> elementType;
        ASTArrayTypeNode(std::unique_ptr<ASTTypeNode> index, std::unique_ptr<ASTTypeNode> element)
            : indexType(std::move(index)), elementType(std::move(element)) {}
};

// Enum
// Production Rule : <enumerated> -> lparent + ident + (comma + ident)* + rparent
// Semantic Rule : enumerated = new ASTEnumeratedTypeNode(elements)
class ASTEnumeratedTypeNode : public ASTTypeNode {
    public:
        std::vector<std::string> elements;
        ASTEnumeratedTypeNode(std::vector<std::string> elements)
            : elements(std::move(elements)) {}
};

// Field Part (Bagian dari Record)
// Production Rule : <field-part> -> <identifier-list> + colon + <type>
// Semantic Rule : field-part = new ASTRecordFieldNode(identifiers, type)
class ASTRecordFieldNode {
    public:
        std::vector<std::string> identifiers;
        std::unique_ptr<ASTTypeNode> type;
        ASTRecordFieldNode(std::vector<std::string> identifiers, std::unique_ptr<ASTTypeNode> type)
            : identifiers(std::move(identifiers)), type(std::move(type)) {}
};

// Record
// Production Rule : <record-type> -> recordsy + <field-list> + endsy
// Semantic Rule : record-type = new ASTRecordTypeNode(fields)
class ASTRecordTypeNode : public ASTTypeNode {
    public:
        std::vector<ASTRecordFieldNode> fields;
        ASTRecordTypeNode(std::vector<ASTRecordFieldNode> fields) 
            : fields(std::move(fields)) {}
};


// expression

// Literal Constant (intcon, realcon, charcon, string)
// Production Rule : <factor> -> intcon | realcon | charcon | string
// Semantic Rule : literal = new ASTLiteralExpressionNode(value)
class ASTLiteralExpressionNode : public ASTExpressionNode {
    public:
        std::variant<int, double, char, std::string> value;
        ASTLiteralExpressionNode(std::variant<int, double, char, std::string> value) 
            : value(value) {}
};

// Component Variable Helper (Untuk array index atau record field)
// Production Rule : <component-variable> -> (lbrack + <index-list> + rbrack) | (period + ident)
// Semantic Rule : component = new ASTVariableComponent(isArrayIndex, fieldName, indices)
class ASTVariableComponent {
    public:
        bool isArrayIndex;
        std::string fieldName;
        std::vector<std::unique_ptr<ASTExpressionNode>> indices;
};

// Variable Access
// Production Rule : <variable> -> ident + (<component-variable>)*
// Semantic Rule : variable = new ASTVariableExpressionNode(baseName, components)
class ASTVariableExpressionNode : public ASTExpressionNode {
    public:
        std::string baseName;
        std::vector<ASTVariableComponent> components;
        ASTVariableExpressionNode(std::string baseName, std::vector<ASTVariableComponent> components)
            : baseName(baseName), components(std::move(components)) {}
};

// Unary Expression (not, +, -)
// Production Rule : <factor> -> (notsy + <factor>) | <simple-expression> -> (plus | minus)? <term>
// Semantic Rule : unary-expr = new ASTUnaryExpressionNode(op, operand)
class ASTUnaryExpressionNode : public ASTExpressionNode {
    public:
        std::string op;
        std::unique_ptr<ASTExpressionNode> operand;
        ASTUnaryExpressionNode(std::string op, std::unique_ptr<ASTExpressionNode> operand)
            : op(op), operand(std::move(operand)) {}
};

// Binary Expression (+, -, *, /, div, mod, and, or, =, <>, <, >, <=, >=)
// Production Rule : <expression> -> <simple-expression> (<relational-operator> <simple-expression>)?
// Semantic Rule : binary-expr = new ASTBinaryExpressionNode(op, lhs, rhs)
class ASTBinaryExpressionNode : public ASTExpressionNode {
    public:
        std::string op;
        std::unique_ptr<ASTExpressionNode> lhs;
        std::unique_ptr<ASTExpressionNode> rhs;
        ASTBinaryExpressionNode(std::string op, std::unique_ptr<ASTExpressionNode> lhs, std::unique_ptr<ASTExpressionNode> rhs)
            : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

// Function/Procedure Call as Expression
// Production Rule : <procedure/function-call> -> ident + (lparent + <parameter-list>? + rparent)?
// Semantic Rule : call-expr = new ASTCallExpressionNode(callee, arguments)
class ASTCallExpressionNode : public ASTExpressionNode {
    public:
        std::string callee;
        std::vector<std::unique_ptr<ASTExpressionNode>> arguments;
        ASTCallExpressionNode(std::string callee, std::vector<std::unique_ptr<ASTExpressionNode>> arguments)
            : callee(callee), arguments(std::move(arguments)) {}
};


// statement

// Compound Statement (Block)
// Production Rule : <compound-statement> -> beginsy + <statement-list> + endsy
// Semantic Rule : compound-stmt = new ASTBlockStatementNode(statements)
class ASTBlockStatementNode : public ASTStatementNode {
    public:
        std::vector<std::unique_ptr<ASTStatementNode>> statements;
        ASTBlockStatementNode(std::vector<std::unique_ptr<ASTStatementNode>> statements) 
            : statements(std::move(statements)) {}
};

// Assignment Statement
// Production Rule : <assignment-statement> -> <variable> + becomes + <expression>
// Semantic Rule : assignment-stmt = new ASTAssignmentStatementNode(target, value)
class ASTAssignmentStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTVariableExpressionNode> target;
        std::unique_ptr<ASTExpressionNode> value;
        ASTAssignmentStatementNode(std::unique_ptr<ASTVariableExpressionNode> target, std::unique_ptr<ASTExpressionNode> value)
            : target(std::move(target)), value(std::move(value)) {}
};

// If Statement
// Production Rule : <if-statement> -> ifsy + <expression> + thensy + <statement> + (elsy + <statement>)?
// Semantic Rule : if-stmt = new ASTIfStatementNode(condition, thenBranch, elseBranch)
class ASTIfStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTExpressionNode> condition;
        std::unique_ptr<ASTStatementNode> thenBranch;
        std::unique_ptr<ASTStatementNode> elseBranch;
        ASTIfStatementNode(std::unique_ptr<ASTExpressionNode> condition, std::unique_ptr<ASTStatementNode> thenBranch, std::unique_ptr<ASTStatementNode> elseBranch = nullptr)
            : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
};

// While Statement
// Production Rule : <while-statement> -> whilesy + <expression> + dosy + <statement>
// Semantic Rule : while-stmt = new ASTWhileStatementNode(condition, body)
class ASTWhileStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTExpressionNode> condition;
        std::unique_ptr<ASTStatementNode> body;
        ASTWhileStatementNode(std::unique_ptr<ASTExpressionNode> condition, std::unique_ptr<ASTStatementNode> body)
            : condition(std::move(condition)), body(std::move(body)) {}
};

// Repeat Statement
// Production Rule : <repeat-statement> -> repeatsy + <statement-list> + untilsy + <expression>
// Semantic Rule : repeat-stmt = new ASTRepeatStatementNode(body, condition)
class ASTRepeatStatementNode : public ASTStatementNode {
    public:
        std::vector<std::unique_ptr<ASTStatementNode>> body;
        std::unique_ptr<ASTExpressionNode> condition;
        ASTRepeatStatementNode(std::vector<std::unique_ptr<ASTStatementNode>> body, std::unique_ptr<ASTExpressionNode> condition)
            : body(std::move(body)), condition(std::move(condition)) {}
};

// For Statement
// Production Rule : <for-statement> -> forsy + ident + becomes + <expression> + (tosy | downtosy) + <expression> + dosy + <statement>
// Semantic Rule : for-stmt = new ASTForStatementNode(iteratorName, startVal, endVal, isDownTo, body)
class ASTForStatementNode : public ASTStatementNode {
    public:
        std::string iteratorName;
        std::unique_ptr<ASTExpressionNode> startVal;
        std::unique_ptr<ASTExpressionNode> endVal;
        bool isDownTo;
        std::unique_ptr<ASTStatementNode> body;
        ASTForStatementNode(std::string iteratorName, std::unique_ptr<ASTExpressionNode> startVal, std::unique_ptr<ASTExpressionNode> endVal, bool isDownTo, std::unique_ptr<ASTStatementNode> body)
            : iteratorName(iteratorName), startVal(std::move(startVal)), endVal(std::move(endVal)), isDownTo(isDownTo), body(std::move(body)) {}
};

// Case Block Helper
// Production Rule : <case-block> -> <constant> + (comma + <constant>)* + colon + <statement> + (semicolon + <case-block>?)* // Semantic Rule : case-branch = new ASTCaseBranchNode(constants, body)
class ASTCaseBranchNode {
    public:
        std::vector<std::unique_ptr<ASTExpressionNode>> constants;
        std::unique_ptr<ASTStatementNode> body;
        ASTCaseBranchNode(std::vector<std::unique_ptr<ASTExpressionNode>> constants, std::unique_ptr<ASTStatementNode> body)
            : constants(std::move(constants)), body(std::move(body)) {}
};

// Case Statement
// Production Rule : <case-statement> -> casesy + <expression> + ofsy + <case-block> + endsy
// Semantic Rule : case-stmt = new ASTCaseStatementNode(condition, branches)
class ASTCaseStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTExpressionNode> condition;
        std::vector<ASTCaseBranchNode> branches;
        ASTCaseStatementNode(std::unique_ptr<ASTExpressionNode> condition, std::vector<ASTCaseBranchNode> branches)
            : condition(std::move(condition)), branches(std::move(branches)) {}
};

// Procedure/Function Call as Statement
// Production Rule : <statement> -> <procedure/function-call>
// Semantic Rule : call-stmt = new ASTCallStatementNode(callExpr)
class ASTCallStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTCallExpressionNode> callExpr;
        ASTCallStatementNode(std::unique_ptr<ASTCallExpressionNode> callExpr) 
            : callExpr(std::move(callExpr)) {}
};


// declaration

// Const Declaration
// Production Rule : <const-declaration> -> constsy + (ident + eql + <constant> + semicolon)+
// Semantic Rule : const-decl = new ASTConstDeclarationNode(name, value)
class ASTConstDeclarationNode : public ASTDeclarationNode {
    public:
        std::string name;
        std::unique_ptr<ASTExpressionNode> value;
        ASTConstDeclarationNode(std::string name, std::unique_ptr<ASTExpressionNode> value)
            : name(name), value(std::move(value)) {}
};

// Type Declaration
// Production Rule : <type-declaration> -> typesy + (ident + eql + <type> + semicolon)+
// Semantic Rule : type-decl = new ASTTypeDeclarationNode(name, typeDefinition)
class ASTTypeDeclarationNode : public ASTDeclarationNode {
    public:
        std::string name;
        std::unique_ptr<ASTTypeNode> typeDefinition;
        ASTTypeDeclarationNode(std::string name, std::unique_ptr<ASTTypeNode> typeDefinition)
            : name(name), typeDefinition(std::move(typeDefinition)) {}
};

// Var Declaration
// Production Rule : <var-declaration> -> varsy + (<identifier-list> + colon + <type> + semicolon)+
// Semantic Rule : var-decl = new ASTVarDeclarationNode(identifiers, type)
class ASTVarDeclarationNode : public ASTDeclarationNode {
    public:
        std::vector<std::string> identifiers;
        std::unique_ptr<ASTTypeNode> type;
        ASTVarDeclarationNode(std::vector<std::string> identifiers, std::unique_ptr<ASTTypeNode> type)
            : identifiers(std::move(identifiers)), type(std::move(type)) {}
};

// Parameter Group Helper (Bagian dari Subprogram)
// Production Rule : <parameter-group> -> <identifier-list> + colon + (ident | <array-type>)
// Semantic Rule : param-group = new ASTParameterGroup(identifiers, type)
class ASTParameterGroup {
    public:
        std::vector<std::string> identifiers;
        std::unique_ptr<ASTTypeNode> type;
        ASTParameterGroup(std::vector<std::string> identifiers, std::unique_ptr<ASTTypeNode> type)
            : identifiers(std::move(identifiers)), type(std::move(type)) {}
};

// Subprogram Declaration Base
class ASTSubprogramDeclarationNode : public ASTDeclarationNode {
    public:
        std::string name;
        std::vector<ASTParameterGroup> parameters;
        std::vector<std::unique_ptr<ASTDeclarationNode>> localDeclarations;
        std::unique_ptr<ASTBlockStatementNode> body;

        ASTSubprogramDeclarationNode(std::string name, std::vector<ASTParameterGroup> parameters, std::vector<std::unique_ptr<ASTDeclarationNode>> localDeclarations, std::unique_ptr<ASTBlockStatementNode> body)
            : name(name), parameters(std::move(parameters)), localDeclarations(std::move(localDeclarations)), body(std::move(body)) {}
};

// Procedure Declaration
// Production Rule : <procedure-declaration> -> proceduresy + ident + (<formal-parameter-list>)? + semicolon + <block> + semicolon
// Semantic Rule : proc-decl = new ASTProcedureDeclarationNode(name, parameters, localDeclarations, body)
class ASTProcedureDeclarationNode : public ASTSubprogramDeclarationNode {
    using ASTSubprogramDeclarationNode::ASTSubprogramDeclarationNode; // Mewarisi constructor
};

// Function Declaration
// Production Rule : <function-declaration> -> functionsy + ident + (<formal-parameter-list>)? + colon + ident + semicolon + <block> + semicolon
// Semantic Rule : func-decl = new ASTFunctionDeclarationNode(name, parameters, returnTypeName, localDeclarations, body)
class ASTFunctionDeclarationNode : public ASTSubprogramDeclarationNode {
    public:
        std::string returnTypeName;
        ASTFunctionDeclarationNode(std::string name, std::vector<ASTParameterGroup> parameters, std::string returnTypeName, std::vector<std::unique_ptr<ASTDeclarationNode>> localDeclarations, std::unique_ptr<ASTBlockStatementNode> body)
            : ASTSubprogramDeclarationNode(name, std::move(parameters), std::move(localDeclarations), std::move(body)), returnTypeName(returnTypeName) {}
};


// Program
// Production Rule : <program> -> <program-header> + <declaration-part> + <compound-statement> + period
// Semantic Rule : program = new ASTProgramNode(programName, declarations, mainBlock)
class ASTProgramNode : public ASTNode {
    public:
        std::string programName;
        std::vector<std::unique_ptr<ASTDeclarationNode>> declarations;
        std::unique_ptr<ASTBlockStatementNode> mainBlock;

        ASTProgramNode(std::string programName, std::vector<std::unique_ptr<ASTDeclarationNode>> declarations, std::unique_ptr<ASTBlockStatementNode> mainBlock)
            : programName(programName), declarations(std::move(declarations)), mainBlock(std::move(mainBlock)) {}
};