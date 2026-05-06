#pragma once

enum class NonTerminal {
    PROGRAM,
    PROGRAM_HEADER,
    DECLARATION_PART,
    CONST_DECLARATION,
    CONSTANT, // 5
    TYPE_DECLARATION,
    VAR_DECLARATION,
    IDENTIFIER_LIST,
    TYPE,
    ARRAY_TYPE, // 10
    RANGE,
    ENUMERATED,
    RECORD_TYPE,
    FIELD_LIST,
    FIELD_PART, // 15
    SUBPROGRAM_DECLARATION,
    PROCEDURE_DECLARATION,
    FUNCTION_DECLARATION,
    BLOCK,
    FORMAL_PARAMETER_LIST, // 20
    PARAMETER_GROUP,
    COMPOUND_STATEMENT,
    STATEMENT_LIST,
    STATEMENT,
    VARIABLE, // 25
    COMPONENT_VARIABLE,
    INDEX_LIST,
    ASSIGNMENT_STATEMENT,
    IF_STATEMENT,
    CASE_STATEMENT, // 30
    CASE_BLOCK,
    WHILE_STATEMENT,
    REPEAT_STATEMENT,
    FOR_STATEMENT,
    PROCEDURE_OR_FUNCTION_CALL, // 30 <procedure/function-call>
    PARAMETER_LIST,
    EXPRESSION,
    SIMPLE_EXPRESSION,
    TERM,
    FACTOR, // 35
    RELATIONAL_OPERATOR,
    ADDITIVE_OPERATOR,
    MULTIPLICATIVE_OPERATOR,
    ERROR, //
};

inline std::string nonTerminalToString(NonTerminal nt) {
    switch (nt) {
        case NonTerminal::PROGRAM: return "program";
        case NonTerminal::PROGRAM_HEADER: return "program-header";
        case NonTerminal::DECLARATION_PART: return "declaration-part";
        case NonTerminal::CONST_DECLARATION: return "const-declaration";
        case NonTerminal::CONSTANT: return "constant";
        case NonTerminal::TYPE_DECLARATION: return "type-declaration";
        case NonTerminal::VAR_DECLARATION: return "var-declaration";
        case NonTerminal::IDENTIFIER_LIST: return "identifier-list";
        case NonTerminal::TYPE: return "type";
        case NonTerminal::ARRAY_TYPE: return "array-type";
        case NonTerminal::RANGE: return "range";
        case NonTerminal::ENUMERATED: return "enumerated";
        case NonTerminal::RECORD_TYPE: return "record-type";
        case NonTerminal::FIELD_LIST: return "field-list";
        case NonTerminal::FIELD_PART: return "field-part";
        case NonTerminal::SUBPROGRAM_DECLARATION: return "subprogram-declaration";
        case NonTerminal::PROCEDURE_DECLARATION: return "procedure-declaration";
        case NonTerminal::FUNCTION_DECLARATION: return "function-declaration";
        case NonTerminal::BLOCK: return "block";
        case NonTerminal::FORMAL_PARAMETER_LIST: return "formal-parameter-list";
        case NonTerminal::PARAMETER_GROUP: return "parameter-group";
        case NonTerminal::COMPOUND_STATEMENT: return "compound-statement";
        case NonTerminal::STATEMENT_LIST: return "statement-list";
        case NonTerminal::STATEMENT: return "statement";
        case NonTerminal::VARIABLE: return "variable";
        case NonTerminal::COMPONENT_VARIABLE: return "component-variable";
        case NonTerminal::INDEX_LIST: return "index-list";
        case NonTerminal::ASSIGNMENT_STATEMENT: return "assignment-statement";
        case NonTerminal::IF_STATEMENT: return "if-statement";
        case NonTerminal::CASE_STATEMENT: return "case-statement";
        case NonTerminal::CASE_BLOCK: return "case-block";
        case NonTerminal::WHILE_STATEMENT: return "while-statement";
        case NonTerminal::REPEAT_STATEMENT: return "repeat-statement";
        case NonTerminal::FOR_STATEMENT: return "for-statement";
        case NonTerminal::PROCEDURE_OR_FUNCTION_CALL: return "procedure/function-call";
        case NonTerminal::PARAMETER_LIST: return "parameter-list";
        case NonTerminal::EXPRESSION: return "expression";
        case NonTerminal::SIMPLE_EXPRESSION: return "simple-expression";
        case NonTerminal::TERM: return "term";
        case NonTerminal::FACTOR: return "factor";
        case NonTerminal::RELATIONAL_OPERATOR: return "relational-operator";
        case NonTerminal::ADDITIVE_OPERATOR: return "additive-operator";
        case NonTerminal::MULTIPLICATIVE_OPERATOR: return "multiplicative-operator";
        default: return "unknown";
    }
}