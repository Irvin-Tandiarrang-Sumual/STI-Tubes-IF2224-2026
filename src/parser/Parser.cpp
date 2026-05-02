#include "Parser.hpp"


// PROGRAM -> PROGRAM-HEADER DECLARATION-PART COMPOUND-STATEMENT period
    // PROGRAM-HEADER -> programsy ident semicolon
    // DECLARATION-PART -> (CONST-DECLARATION)* (TYPE-DECLARATION)* (VAR-DECLARATION)* (SUBPROGRAM-DECLARATION)*
        // CONST-DECLARATION -> constsy (ident eql CONSTANT semicolon)
            // CONSTANT -> charcon | string | [(plus | minus)? (ident|intcon|realcon)]
        // TYPE-DECLARATION -> typesy (ident eql TYPE semicolon)
        // VAR-DECLARATION -> varsy (IDENTIFIER-LIST colon TYPE semicolon)
            // IDENTIFIER-LIST -> ident (comma ident)*
            // TYPE -> ident | ARRAY-TYPE | RANGE | ENUMERATED | RECORD-TYPE
                // ARRAY-TYPE -> arraysy lbrack (RANGE | ident) rbrack ofsy TYPE
                // RANGE -> EXPRESSION + period + period + EXPRESSION
                // ENUMERATED -> lparent ident (comma ident)* rparent
                // RECORD-TYPE -> recordsy FIELD-LIST endsy
                    // FIELD-LIST -> FIELD-PART (semicolon FIELD-PART)*
                        // FIELD-PART -> IDENTIFIER-LIST colon TYPE
        // SUBPROGRAM-DECLARATION -> PROCEDURE-DECLARATION | FUNCTION-DECLARATION
            // PROCEDURE-DECLARATION -> proceduresy ident (FORMAL-PARAMETER-LIST)? semicolon BLOCK semicolon
            // FUNCTION-DECLARATION -> functionsy ident (FORMAL-PARAMETER-LIST)? colon ident semicolon BLOCK semicolon
                // BLOCK -> DECLARATION-PART COMPOUND-STATEMENT
                // FORMAL-PARAMETER-LIST -> lparent PARAMETER-GROUP (semicolon PARAMETER-GROUP)* rparent
                    // PARAMETER-GROUP -> IDENTIFIER-LIST colon (ident|ARRAY-TYPE)
                        // COMPOUND-STATEMENT -> beginsy STATEMENT-LIST endsy
                        // STATEMENT-LIST -> STATEMENT (semicolon STATEMENT)*
// STATEMENT -> (ASSIGNMENT-STATEMENT | IF-STATEMENT | CASE-STATEMENT | WHILE-STATEMENT | REPEAT-STATEMENT | FOR-STATEMENT)? | PROCEDURE/FUNCTION-CALL
    // ASSIGNMENT-STATEMENT -> ident becomes EXPRESSION
    // IF-STATEMENT -> ifsy EXPRESSION thensy STATEMENT (elsy STATEMENT)?
    // CASE-STATEMENT -> casesy EXPRESSION ofsy CASE-BLOCK endsy
// CASE-BLOCK -> CONSTANT (comma CONSTANT)* colon STATEMENT (semicolon CASE-BLOCK?)*
    // WHILE-STATEMENT -> whilesy EXPRESSION dosy STATEMENT
    // REPEAT-STATEMENT -> repeatsy STATEMENT-LIST untilsy EXPRESSION
    // FOR-STATEMENT -> forsy ident becomes EXPRESSION (tosy | downtosy) EXPRESSION dosy STATEMENT
    // PROCEDURE/FUNCTION-CALL -> ident (lparent PARAMETER-LIST rparent)?
// PARAMETER-LIST -> EXPRESSION (comma EXPRESSION)*
    // EXPRESSION -> SIMPLE-EXPRESSION (RELATIONAL-OPERATOR SIMPLE-EXPRESSION)? -------------------?????????????????????????????????????????????
    // SIMPLE-EXPRESSION -> (plus | minus)? TERM (ADDITIVE-OPERATOR TERM)* -------------------?????????????????????????????????????????????
    // TERM -> FACTOR (MULTIPLICATIVE-OPERATOR factor)* -------------------?????????????????????????????????????????????
    // RELATIONAL-OPERATOR -> eql | neq | gtr | geq | lss | leq
    // ADDITIVE-OPERATOR -> plus | minus | orsy
    // MULTIPLICATIVE-OPERATOR -> times | rdiv | idiv | imod |andsy



// FACTOR -> ident FACTOR-HELPER | intcon | charcon | string | (lparent EXPRESSION rparent) | notsy FACTOR |
// FACTOR-HELPER -> epsilon | lparent PARAMETER-LIST rparent

// STATEMENT -> ident STATEMENT-HELPER | IF-STATEMENT | CASE-STATEMENT | WHILE-STATEMENT | REPEAT-STATEMENT | FOR-STATEMENT
// STATEMENT-HELPER -> becomes EXPRESSION | lparent PARAMETER-LIST rparent | epsilon



// problem :

// STATEMENT -> (ASSIGNMENT-STATEMENT | IF-STATEMENT | CASE-STATEMENT | WHILE-STATEMENT | REPEAT-STATEMENT | FOR-STATEMENT)? | PROCEDURE/FUNCTION-CALL
// PROCEDURE/FUNCTION-CALL -> ident (lparent PARAMETER-LIST rparent)?
// ASSIGNMENT-STATEMENT -> ident becomes EXPRESSION
// FACTOR -> ident | intcon | charcon | string | (lparent EXPRESSION rparent) | notsy FACTOR | PROCEDURE/FUNCTIONAL-CALL

// FACTOR -> ident | intcon | charcon | string | (lparent EXPRESSION rparent) | notsy FACTOR | PROCEDURE/FUNCTIONAL-CALL
// STATEMENT -> (ASSIGNMENT-STATEMENT | IF-STATEMENT | CASE-STATEMENT | WHILE-STATEMENT | REPEAT-STATEMENT | FOR-STATEMENT)? | PROCEDURE/FUNCTION-CALL

