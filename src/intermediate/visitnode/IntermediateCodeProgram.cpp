#include "../IntermediateCode.hpp"

std::any IntermediateCodeGenerator::visitProgramNode(ASTProgramNode* node) {
    emitInt(computeProgramMemorySize(node));

    if (node != nullptr && node->declarations != nullptr) {
        node->declarations->accept(this);
    }

    if (node != nullptr && node->mainBlock != nullptr) {
        node->mainBlock->accept(this);
    }

    emitRet();
    return {};
}