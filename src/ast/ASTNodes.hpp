#pragma once
#include "../reader/CodeLocation.hpp"
#include <vector>
#include <memory>
#include <string>
#include <variant>
#include <type_traits>

inline std::string astVariantToString(const std::variant<int, double, char, bool, std::string>& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, char>) {
            return std::string(1, arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return arg;
        }
    }, value);
}

class ASTNode {
    public:
        CodeLocation location_;
        std::vector<ASTNode*> children_;
        ASTNode* parent_ = nullptr;
        virtual ~ASTNode() = default;

        virtual std::string toString() const {
            return "ASTNode";
        }
};

class ASTTypeNode : public ASTNode {
    public:
        bool isAnonymous = false;
};
class ASTExpressionNode : public ASTNode {};
class ASTStatementNode : public ASTNode {};
class ASTDeclarationNode : public ASTNode {};


// type

// Primitive Type
// Contoh : real, integer, char, boolean, string
class ASTPrimitiveType : public ASTTypeNode {
    public:
        std::string type;
        ASTPrimitiveType(std::string type) : type(std::move(type)) {}

        std::string toString() const override {
            return "PrimitiveType: " + type;
        }
};

class ASTNamedTypeNode : public ASTTypeNode {
    public:
        std::string typeName;
        ASTNamedTypeNode(std::string typeName) : typeName(std::move(typeName)) {}

        std::string toString() const override {
            return "NamedType: " + typeName;
        }
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

        std::string toString() const override {
            return "RangeType";
        }
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

        std::string toString() const override {
            return "ArrayType";
        }
};

// Enum
// Production Rule : <enumerated> -> lparent + ident + (comma + ident)* + rparent
// Semantic Rule : enumerated = new ASTEnumeratedTypeNode(elements)
class ASTEnumeratedTypeNode : public ASTTypeNode {
    public:
        std::vector<std::string> elements;
        ASTEnumeratedTypeNode(std::vector<std::string> elements)
            : elements(std::move(elements)) {}

        std::string toString() const override {
            return "EnumeratedType";
        }
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

        std::string toString() const override {
            return "RecordType";
        }
};


// expression

// Literal Constant (intcon, realcon, charcon, string)
// Production Rule : <factor> -> intcon | realcon | charcon | string
// Semantic Rule : literal = new ASTLiteralExpressionNode(value)
class ASTLiteralExpressionNode : public ASTExpressionNode {
    public:
        std::variant<int, double, char, bool, std::string> value;
        ASTLiteralExpressionNode(std::variant<int, double, char, bool, std::string> value) 
            : value(std::move(value)) {}

        std::string toString() const override {
            return "Literal: " + astVariantToString(value);
        }
};

// Component Variable Helper (Untuk array index atau record field)
// Production Rule : <component-variable> -> (lbrack + <index-list> + rbrack) | (period + ident)
// Semantic Rule : component = new ASTVariableComponent(isArrayIndex, fieldName, indices)
class ASTVariableComponent {
    public:
        bool isArrayIndex = false;
        std::string fieldName;
        std::vector<std::unique_ptr<ASTExpressionNode>> indices;

        ASTVariableComponent() = default;

        explicit ASTVariableComponent(std::string fieldName)
            : isArrayIndex(false), fieldName(std::move(fieldName)) {}

        explicit ASTVariableComponent(std::vector<std::unique_ptr<ASTExpressionNode>> indices)
            : isArrayIndex(true), indices(std::move(indices)) {}

        ASTVariableComponent(ASTVariableComponent&&) noexcept = default;
        ASTVariableComponent& operator=(ASTVariableComponent&&) noexcept = default;
        ASTVariableComponent(const ASTVariableComponent&) = delete;
        ASTVariableComponent& operator=(const ASTVariableComponent&) = delete;
};

// Variable Access
// Production Rule : <variable> -> ident + (<component-variable>)*
// Semantic Rule : variable = new ASTVariableExpressionNode(baseName, components)
class ASTVariableExpressionNode : public ASTExpressionNode {
    public:
        std::string baseName;
        std::vector<ASTVariableComponent> components;
        ASTVariableExpressionNode(std::string baseName, std::vector<ASTVariableComponent> components)
            : baseName(std::move(baseName)), components(std::move(components)) {}

        std::string toString() const override {
            return "Variable: " + baseName;
        }
};

// Unary Expression (not, +, -)
// Production Rule : <factor> -> (notsy + <factor>) | <simple-expression> -> (plus | minus)? <term>
// Semantic Rule : unary-expr = new ASTUnaryExpressionNode(op, operand)
class ASTUnaryExpressionNode : public ASTExpressionNode {
    public:
        std::string op;
        std::unique_ptr<ASTExpressionNode> operand;
        ASTUnaryExpressionNode(std::string op, std::unique_ptr<ASTExpressionNode> operand)
            : op(std::move(op)), operand(std::move(operand)) {}

        std::string toString() const override {
            return "UnaryOp: " + op;
        }
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
            : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

        std::string toString() const override {
            return "BinaryOp: " + op;
        }
};

// Function/Procedure Call as Expression
// Production Rule : <procedure/function-call> -> ident + (lparent + <parameter-list>? + rparent)?
// Semantic Rule : call-expr = new ASTCallExpressionNode(callee, arguments)
class ASTCallExpressionNode : public ASTExpressionNode {
    public:
        std::string callee;
        std::vector<std::unique_ptr<ASTExpressionNode>> arguments;
        ASTCallExpressionNode(std::string callee, std::vector<std::unique_ptr<ASTExpressionNode>> arguments)
            : callee(std::move(callee)), arguments(std::move(arguments)) {}

        std::string toString() const override {
            return "Call: " + callee;
        }
};


// statement

// Empty Statement
// Production Rule : <statement> -> ε
// Semantic Rule : empty-stmt = new ASTEmptyStatementNode()
class ASTEmptyStatementNode : public ASTStatementNode {
    public:
        ASTEmptyStatementNode() = default;

        std::string toString() const override {
            return "EmptyStatement";
        }
};

// Compound Statement (Block)
// Production Rule : <compound-statement> -> beginsy + <statement-list> + endsy
// Semantic Rule : compound-stmt = new ASTBlockStatementNode(statements)
class ASTBlockStatementNode : public ASTStatementNode {
    public:
        std::vector<std::unique_ptr<ASTStatementNode>> statements;
        ASTBlockStatementNode(std::vector<std::unique_ptr<ASTStatementNode>> statements) 
            : statements(std::move(statements)) {}

        std::string toString() const override {
            return "Block";
        }
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

        std::string toString() const override {
            return "Assignment";
        }
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

        std::string toString() const override {
            return "If";
        }
};

// While Statement
// Production Rule : <while-statement> -> whilesy + <expression> + dosy + <compound-statement> + semicolon
// Semantic Rule : while-stmt = new ASTWhileStatementNode(condition, body)
class ASTWhileStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTExpressionNode> condition;
        std::unique_ptr<ASTBlockStatementNode> body;
        ASTWhileStatementNode(std::unique_ptr<ASTExpressionNode> condition, std::unique_ptr<ASTBlockStatementNode> body)
            : condition(std::move(condition)), body(std::move(body)) {}

        std::string toString() const override {
            return "While";
        }
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

        std::string toString() const override {
            return "Repeat";
        }
};

// For Statement
// Production Rule : <for-statement> -> forsy + ident + becomes + <expression> + (tosy | downtosy) + <expression> + dosy + <compound-statement> + semicolon
// Semantic Rule : for-stmt = new ASTForStatementNode(iteratorName, startVal, endVal, isDownTo, body)
class ASTForStatementNode : public ASTStatementNode {
    public:
        std::string iteratorName;
        std::unique_ptr<ASTExpressionNode> startVal;
        std::unique_ptr<ASTExpressionNode> endVal;
        bool isDownTo;
        std::unique_ptr<ASTBlockStatementNode> body;
        ASTForStatementNode(std::string iteratorName, std::unique_ptr<ASTExpressionNode> startVal, std::unique_ptr<ASTExpressionNode> endVal, bool isDownTo, std::unique_ptr<ASTBlockStatementNode> body)
            : iteratorName(std::move(iteratorName)), startVal(std::move(startVal)), endVal(std::move(endVal)), isDownTo(isDownTo), body(std::move(body)) {}

        std::string toString() const override {
            return "For: " + iteratorName;
        }
};

// Case Block Helper
// Production Rule : <case-block> -> <constant> + (comma + <constant>)* + colon + <statement> + (semicolon + <case-block>?)* // Semantic Rule : case-branch = new ASTCaseBranchNode(constants, body)
class ASTCaseBranchNode {
    public:
        std::vector<std::unique_ptr<ASTExpressionNode>> constants;
        std::unique_ptr<ASTStatementNode> body;
        ASTCaseBranchNode(std::vector<std::unique_ptr<ASTExpressionNode>> constants, std::unique_ptr<ASTStatementNode> body)
            : constants(std::move(constants)), body(std::move(body)) {}

        ASTCaseBranchNode(ASTCaseBranchNode&&) noexcept = default;
        ASTCaseBranchNode& operator=(ASTCaseBranchNode&&) noexcept = default;
        ASTCaseBranchNode(const ASTCaseBranchNode&) = delete;
        ASTCaseBranchNode& operator=(const ASTCaseBranchNode&) = delete;
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

        std::string toString() const override {
            return "Case";
        }
};

// Procedure/Function Call as Statement
// Production Rule : <statement> -> <procedure/function-call>
// Semantic Rule : call-stmt = new ASTCallStatementNode(callExpr)
class ASTCallStatementNode : public ASTStatementNode {
    public:
        std::unique_ptr<ASTCallExpressionNode> callExpr;
        ASTCallStatementNode(std::unique_ptr<ASTCallExpressionNode> callExpr) 
            : callExpr(std::move(callExpr)) {}

        std::string toString() const override {
            return "CallStatement";
        }
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
            : name(std::move(name)), value(std::move(value)) {}

        std::string toString() const override {
            return "ConstDeclaration: " + name;
        }
};

// Type Declaration
// Production Rule : <type-declaration> -> typesy + (ident + eql + <type> + semicolon)+
// Semantic Rule : type-decl = new ASTTypeDeclarationNode(name, typeDefinition)
class ASTTypeDeclarationNode : public ASTDeclarationNode {
    public:
        std::string name;
        std::unique_ptr<ASTTypeNode> typeDefinition;
        ASTTypeDeclarationNode(std::string name, std::unique_ptr<ASTTypeNode> typeDefinition)
            : name(std::move(name)), typeDefinition(std::move(typeDefinition)) {}

        std::string toString() const override {
            return "TypeDeclaration: " + name;
        }
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

        std::string toString() const override {
            return "VarDeclaration";
        }
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

        ASTParameterGroup(ASTParameterGroup&&) noexcept = default;
        ASTParameterGroup& operator=(ASTParameterGroup&&) noexcept = default;
        ASTParameterGroup(const ASTParameterGroup&) = delete;
        ASTParameterGroup& operator=(const ASTParameterGroup&) = delete;
};

// Subprogram Declaration Base
class ASTSubprogramDeclarationNode : public ASTDeclarationNode {
    public:
        std::string name;
        std::vector<ASTParameterGroup> parameters;
        std::vector<std::unique_ptr<ASTDeclarationNode>> localDeclarations;
        std::unique_ptr<ASTBlockStatementNode> body;

        ASTSubprogramDeclarationNode(std::string name, std::vector<ASTParameterGroup> parameters, std::vector<std::unique_ptr<ASTDeclarationNode>> localDeclarations, std::unique_ptr<ASTBlockStatementNode> body)
            : name(std::move(name)), parameters(std::move(parameters)), localDeclarations(std::move(localDeclarations)), body(std::move(body)) {}

        std::string toString() const override {
            return "SubprogramDeclaration: " + name;
        }
};

// Procedure Declaration
// Production Rule : <procedure-declaration> -> proceduresy + ident + (<formal-parameter-list>)? + semicolon + <block> + semicolon
// Semantic Rule : proc-decl = new ASTProcedureDeclarationNode(name, parameters, localDeclarations, body)
class ASTProcedureDeclarationNode : public ASTSubprogramDeclarationNode {
    public:
        using ASTSubprogramDeclarationNode::ASTSubprogramDeclarationNode; // Mewarisi constructor
};

// Function Declaration
// Production Rule : <function-declaration> -> functionsy + ident + (<formal-parameter-list>)? + colon + ident + semicolon + <block> + semicolon
// Semantic Rule : func-decl = new ASTFunctionDeclarationNode(name, parameters, returnTypeName, localDeclarations, body)
class ASTFunctionDeclarationNode : public ASTSubprogramDeclarationNode {
    public:
        std::string returnTypeName;
        ASTFunctionDeclarationNode(std::string name, std::vector<ASTParameterGroup> parameters, std::string returnTypeName, std::vector<std::unique_ptr<ASTDeclarationNode>> localDeclarations, std::unique_ptr<ASTBlockStatementNode> body)
            : ASTSubprogramDeclarationNode(name, std::move(parameters), std::move(localDeclarations), std::move(body)), returnTypeName(std::move(returnTypeName)) {}

        std::string toString() const override {
            return "FunctionDeclaration: " + name + " : " + returnTypeName;
        }
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
            : programName(std::move(programName)), declarations(std::move(declarations)), mainBlock(std::move(mainBlock)) {}

        std::string toString() const override {
            return "Program: " + programName;
        }
};