#pragma once
#include <any>
#include "ASTVisitor.hpp"
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
        virtual ~ASTNode() {
            for (ASTNode* child : children_) {
                delete child;
            }
        }

        virtual std::any accept(ASTVisitor *visitor) = 0;

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
        ASTPrimitiveType(std::string type) : type(type) {}

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visit(this);
        }

        std::string toString() const override {
            return "PrimitiveType: " + type;
        }
};

class ASTNamedTypeNode : public ASTTypeNode {
    public:
        std::string typeName;
        ASTNamedTypeNode(std::string typeName) : typeName(std::move(typeName)) {}

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitNamedTypeNode(this);
        }

        std::string toString() const override {
            return "NamedType: " + typeName;
        }
};


// Range
// Production Rule : <range> -> <constant> + period + period + <constant>
// Semantic Rule : range = new ASTRangeType(startConstant, endConstant)
class ASTRangeType : public ASTTypeNode {
    public:
        ASTExpressionNode* startConstant;
        ASTExpressionNode* endConstant;
        ASTRangeType(ASTExpressionNode* startConstant, ASTExpressionNode* endConstant)
            : startConstant(startConstant), endConstant(endConstant) {
            if (startConstant != nullptr) children_.push_back(startConstant);
            if (endConstant != nullptr) children_.push_back(endConstant);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitRangeType(this);
        }

        std::string toString() const override {
            return "RangeType";
        }
};

// Array
// Production Rule : <array-type> -> arraysy + lbrack + (<range> | ident) + rbrack + ofsy + <type>
// Semantic Rule : array-type = new ASTArrayTypeNode(indexType, elementType)
class ASTArrayTypeNode : public ASTTypeNode {
    public:
        ASTTypeNode* indexType;
        ASTTypeNode* elementType;
        ASTArrayTypeNode(ASTTypeNode* index, ASTTypeNode* element)
            : indexType(index), elementType(element) {
            if (indexType != nullptr) children_.push_back(indexType);
            if (elementType != nullptr) children_.push_back(elementType);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitArrayTypeNode(this);
        }

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

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitEnumeratedTypeNode(this);
        }

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
        ASTTypeNode* type;
        ASTRecordFieldNode(std::vector<std::string> identifiers, ASTTypeNode* type)
            : identifiers(std::move(identifiers)), type(type) {}
};

// Record
// Production Rule : <record-type> -> recordsy + <field-list> + endsy
// Semantic Rule : record-type = new ASTRecordTypeNode(fields)
class ASTRecordTypeNode : public ASTTypeNode {
    public:
        std::vector<ASTRecordFieldNode> fields;
        ASTRecordTypeNode(std::vector<ASTRecordFieldNode> fields) 
            : fields(std::move(fields)) {
            for (auto& field : this->fields) {
                if (field.type != nullptr) children_.push_back(field.type);
            }
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitRecordTypeNode(this);
        }

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
            : value(value) {}

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitLiteralExpressionNode(this);
        }

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
        std::vector<ASTExpressionNode*> indices;

        ASTVariableComponent() = default;

        explicit ASTVariableComponent(std::string fieldName)
            : isArrayIndex(false), fieldName(std::move(fieldName)) {}

        explicit ASTVariableComponent(std::vector<ASTExpressionNode*> indices)
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
            : baseName(std::move(baseName)), components(std::move(components)) {
            for (auto& component : this->components) {
                for (auto* index : component.indices) {
                    if (index != nullptr) children_.push_back(index);
                }
            }
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitVariableExpressionNode(this);
        }

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
        ASTExpressionNode* operand;
        ASTUnaryExpressionNode(std::string op, ASTExpressionNode* operand)
            : op(std::move(op)), operand(operand) {
            if (operand != nullptr) children_.push_back(operand);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitUnaryExpressionNode(this);
        }

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
        ASTExpressionNode* lhs;
        ASTExpressionNode* rhs;
        ASTBinaryExpressionNode(std::string op, ASTExpressionNode* lhs, ASTExpressionNode* rhs)
            : op(std::move(op)), lhs(lhs), rhs(rhs) {
            if (lhs != nullptr) children_.push_back(lhs);
            if (rhs != nullptr) children_.push_back(rhs);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitBinaryExpressionNode(this);
        }

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
        std::vector<ASTExpressionNode*> arguments;
        ASTCallExpressionNode(std::string callee, std::vector<ASTExpressionNode*> arguments)
            : callee(std::move(callee)), arguments(std::move(arguments)) {
            for (auto* arg : this->arguments) {
                if (arg != nullptr) children_.push_back(arg);
            }
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitCallExpressionNode(this);
        }

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

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitEmptyStatementNode(this);
        }

        std::string toString() const override {
            return "EmptyStatement";
        }
};

// Compound Statement (Block)
// Production Rule : <compound-statement> -> beginsy + <statement-list> + endsy
// Semantic Rule : compound-stmt = new ASTBlockStatementNode(statements)
class ASTBlockStatementNode : public ASTStatementNode {
    public:
        std::vector<ASTStatementNode*> statements;
        ASTBlockStatementNode(std::vector<ASTStatementNode*> statements) 
            : statements(std::move(statements)) {
            for (auto* statement : this->statements) {
                if (statement != nullptr) children_.push_back(statement);
            }
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitBlockStatementNode(this);
        }

        std::string toString() const override {
            return "Block";
        }
};

// Assignment Statement
// Production Rule : <assignment-statement> -> <variable> + becomes + <expression>
// Semantic Rule : assignment-stmt = new ASTAssignmentStatementNode(target, value)
class ASTAssignmentStatementNode : public ASTStatementNode {
    public:
        ASTVariableExpressionNode* target;
        ASTExpressionNode* value;
        ASTAssignmentStatementNode(ASTVariableExpressionNode* target, ASTExpressionNode* value)
            : target(target), value(value) {
            if (target != nullptr) children_.push_back(target);
            if (value != nullptr) children_.push_back(value);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitAssignmentStatementNode(this);
        }

        std::string toString() const override {
            return "Assignment";
        }
};

// If Statement
// Production Rule : <if-statement> -> ifsy + <expression> + thensy + <statement> + (elsy + <statement>)?
// Semantic Rule : if-stmt = new ASTIfStatementNode(condition, thenBranch, elseBranch)
class ASTIfStatementNode : public ASTStatementNode {
    public:
        ASTExpressionNode* condition;
        ASTStatementNode* thenBranch;
        ASTStatementNode* elseBranch;
        ASTIfStatementNode(ASTExpressionNode* condition, ASTStatementNode* thenBranch, ASTStatementNode* elseBranch = nullptr)
            : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
            if (condition != nullptr) children_.push_back(condition);
            if (thenBranch != nullptr) children_.push_back(thenBranch);
            if (elseBranch != nullptr) children_.push_back(elseBranch);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitIfStatementNode(this);
        }

        std::string toString() const override {
            return "If";
        }
};

// While Statement
// Production Rule : <while-statement> -> whilesy + <expression> + dosy + <compound-statement> + semicolon
// Semantic Rule : while-stmt = new ASTWhileStatementNode(condition, body)
class ASTWhileStatementNode : public ASTStatementNode {
    public:
        ASTExpressionNode* condition;
        ASTBlockStatementNode* body;
        ASTWhileStatementNode(ASTExpressionNode* condition, ASTBlockStatementNode* body)
            : condition(condition), body(body) {
            if (condition != nullptr) children_.push_back(condition);
            if (body != nullptr) children_.push_back(body);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitWhileStatementNode(this);
        }

        std::string toString() const override {
            return "While";
        }
};

// Repeat Statement
// Production Rule : <repeat-statement> -> repeatsy + <statement-list> + untilsy + <expression>
// Semantic Rule : repeat-stmt = new ASTRepeatStatementNode(body, condition)
class ASTRepeatStatementNode : public ASTStatementNode {
    public:
        std::vector<ASTStatementNode*> body;
        ASTExpressionNode* condition;
        ASTRepeatStatementNode(std::vector<ASTStatementNode*> body, ASTExpressionNode* condition)
            : body(std::move(body)), condition(condition) {
            for (auto* statement : this->body) {
                if (statement != nullptr) children_.push_back(statement);
            }
            if (condition != nullptr) children_.push_back(condition);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitRepeatStatementNode(this);
        }

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
        ASTExpressionNode* startVal;
        ASTExpressionNode* endVal;
        bool isDownTo;
        ASTBlockStatementNode* body;
        ASTForStatementNode(std::string iteratorName, ASTExpressionNode* startVal, ASTExpressionNode* endVal, bool isDownTo, ASTBlockStatementNode* body)
            : iteratorName(std::move(iteratorName)), startVal(startVal), endVal(endVal), isDownTo(isDownTo), body(body) {
            if (startVal != nullptr) children_.push_back(startVal);
            if (endVal != nullptr) children_.push_back(endVal);
            if (body != nullptr) children_.push_back(body);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitForStatementNode(this);
        }

        std::string toString() const override {
            return "For: " + iteratorName;
        }
};

// Case Block Helper
// Production Rule : <case-block> -> <constant> + (comma + <constant>)* + colon + <statement> + (semicolon + <case-block>?)* // Semantic Rule : case-branch = new ASTCaseBranchNode(constants, body)
class ASTCaseBranchNode {
    public:
        std::vector<ASTExpressionNode*> constants;
        ASTStatementNode* body;
        ASTCaseBranchNode(std::vector<ASTExpressionNode*> constants, ASTStatementNode* body)
            : constants(std::move(constants)), body(body) {}

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
        ASTExpressionNode* condition;
        std::vector<ASTCaseBranchNode> branches;
        ASTCaseStatementNode(ASTExpressionNode* condition, std::vector<ASTCaseBranchNode> branches)
            : condition(condition), branches(std::move(branches)) {
            if (condition != nullptr) children_.push_back(condition);
            for (auto& branch : this->branches) {
                for (auto* constant : branch.constants) {
                    if (constant != nullptr) children_.push_back(constant);
                }
                if (branch.body != nullptr) children_.push_back(branch.body);
            }
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitCaseStatementNode(this);
        }

        std::string toString() const override {
            return "Case";
        }
};

// Procedure/Function Call as Statement
// Production Rule : <statement> -> <procedure/function-call>
// Semantic Rule : call-stmt = new ASTCallStatementNode(callExpr)
class ASTCallStatementNode : public ASTStatementNode {
    public:
        ASTCallExpressionNode* callExpr;
        ASTCallStatementNode(ASTCallExpressionNode* callExpr) 
            : callExpr(callExpr) {
            if (callExpr != nullptr) children_.push_back(callExpr);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitCallStatementNode(this);
        }

        std::string toString() const override {
            return "CallStatement";
        }
};


// declaration
class ASTDeclarationsNode : public ASTNode {
public:
    std::vector<ASTDeclarationNode*> declarations;

    ASTDeclarationsNode(std::vector<ASTDeclarationNode*> declarations)
        : declarations(std::move(declarations)) {

        for (ASTDeclarationNode* declaration : this->declarations) {
            if (declaration != nullptr) {
                declaration->parent_ = this;
                children_.push_back(declaration);
            }
        }
    }

    std::any accept(ASTVisitor *visitor) override {
        return visitor->visitDeclarationsNode(this);
    }

    std::string toString() const override {
        return "Declarations";
    }
};

// Const Declaration
// Production Rule : <const-declaration> -> constsy + (ident + eql + <constant> + semicolon)+
// Semantic Rule : const-decl = new ASTConstDeclarationNode(name, value)
class ASTConstDeclarationNode : public ASTDeclarationNode {
    public:
        std::string name;
        ASTExpressionNode* value;
        ASTConstDeclarationNode(std::string name, ASTExpressionNode* value)
            : name(std::move(name)), value(value) {
            if (value != nullptr) children_.push_back(value);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitConstDeclarationNode(this);
        }

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
        ASTTypeNode* typeDefinition;
        ASTTypeDeclarationNode(std::string name, ASTTypeNode* typeDefinition)
            : name(std::move(name)), typeDefinition(typeDefinition) {
            if (typeDefinition != nullptr) children_.push_back(typeDefinition);
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitTypeDeclarationNode(this);
        }

        std::string toString() const override {
            return "TypeDeclaration: " + name;
        }
};

// Var Declaration
// Production Rule : <var-declaration> -> varsy + (<identifier-list> + colon + <type> + semicolon)+
// Semantic Rule : var-decl = new ASTVarDeclarationNode(identifiers, type)
class ASTVarDeclarationNode : public ASTDeclarationNode {
    public:
        std::string identifiers;
        ASTTypeNode* type;

        ASTVarDeclarationNode(std::string name, ASTTypeNode* type)
            : identifiers(std::move(name)), type(type) {}

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitVarDeclarationNode(this);
        }
        
        std::string toString() const override {
            std::string typeText = "unknown";
            if (type != nullptr) {
                typeText = type->toString();
                const std::string primitivePrefix = "PrimitiveType: ";
                if (typeText.rfind(primitivePrefix, 0) == 0) {
                    typeText = typeText.substr(primitivePrefix.size());
                }
                const std::string namedPrefix = "NamedType: ";
                if (typeText.rfind(namedPrefix, 0) == 0) {
                    typeText = typeText.substr(namedPrefix.size());
                }
            }
            return "VarDecl(name: '" + identifiers + "', type: '" + typeText + "')";
        }

};

// Parameter Group Helper (Bagian dari Subprogram)
// Production Rule : <parameter-group> -> <identifier-list> + colon + (ident | <array-type>)
// Semantic Rule : param-group = new ASTParameterGroup(identifiers, type)
class ASTParameterGroup {
    public:
        std::vector<std::string> identifiers;
        ASTTypeNode* type;
        ASTParameterGroup(std::vector<std::string> identifiers, ASTTypeNode* type)
            : identifiers(std::move(identifiers)), type(type) {}

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
        std::vector<ASTDeclarationNode*> localDeclarations;
        ASTBlockStatementNode* body;

        ASTSubprogramDeclarationNode(std::string name, std::vector<ASTParameterGroup> parameters, std::vector<ASTDeclarationNode*> localDeclarations, ASTBlockStatementNode* body)
            : name(std::move(name)), parameters(std::move(parameters)), localDeclarations(std::move(localDeclarations)), body(body) {
            for (auto& parameter : this->parameters) {
                if (parameter.type != nullptr) children_.push_back(parameter.type);
            }
            for (auto* declaration : this->localDeclarations) {
                if (declaration != nullptr) children_.push_back(declaration);
            }
            if (body != nullptr) children_.push_back(body);
        }

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

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitProcedureDeclarationNode(this);
        }
};

// Function Declaration
// Production Rule : <function-declaration> -> functionsy + ident + (<formal-parameter-list>)? + colon + ident + semicolon + <block> + semicolon
// Semantic Rule : func-decl = new ASTFunctionDeclarationNode(name, parameters, returnTypeName, localDeclarations, body)
class ASTFunctionDeclarationNode : public ASTSubprogramDeclarationNode {
    public:
        std::string returnTypeName;
        ASTFunctionDeclarationNode(std::string name, std::vector<ASTParameterGroup> parameters, std::string returnTypeName, std::vector<ASTDeclarationNode*> localDeclarations, ASTBlockStatementNode* body)
            : ASTSubprogramDeclarationNode(name, std::move(parameters), std::move(localDeclarations), body), returnTypeName(std::move(returnTypeName)) {}

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitFunctionDeclarationNode(this);
        }

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
        ASTDeclarationsNode* declarations;
        ASTBlockStatementNode* mainBlock;

        ASTProgramNode(std::string programName, ASTDeclarationsNode* declarations, ASTBlockStatementNode* mainBlock)
            : programName(std::move(programName)), declarations(std::move(declarations)), mainBlock(mainBlock) {
            if (declarations != nullptr) {
                declarations->parent_ = this;
                children_.push_back(declarations);
            }

            if (mainBlock != nullptr) {
                mainBlock->parent_ = this;
                children_.push_back(mainBlock);
            }
        }

        std::any accept(ASTVisitor *visitor) override {
            return visitor->visitProgramNode(this);
        }

        std::string toString() const override {
            return "Program: " + programName;
        }
};