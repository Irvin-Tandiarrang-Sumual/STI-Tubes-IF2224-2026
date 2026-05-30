#include "../IntermediateCode.hpp"

// Types Block
std::any IntermediateCodeGenerator::visitTypeNode(ASTTypeNode* node) {
    return {}; 
}

// Tipe primitif (Integer, Boolean, Char)
std::any IntermediateCodeGenerator::visitPrimitiveType(ASTPrimitiveType* node) {
    return {};
}

// Alias tipe data
std::any IntermediateCodeGenerator::visitNamedTypeNode(ASTNamedTypeNode* node) {
    return {};
}

// Range type (misal 1..10)
std::any IntermediateCodeGenerator::visitRangeType(ASTRangeType* node) {
    return {};
}

// Definisi Array
std::any IntermediateCodeGenerator::visitArrayTypeNode(ASTArrayTypeNode* node) {
    return {};
}

// Definisi Enum
std::any IntermediateCodeGenerator::visitEnumeratedTypeNode(ASTEnumeratedTypeNode* node) {
    return {};
}

// Definisi Record
std::any IntermediateCodeGenerator::visitRecordTypeNode(ASTRecordTypeNode* node) {
    return {};
}