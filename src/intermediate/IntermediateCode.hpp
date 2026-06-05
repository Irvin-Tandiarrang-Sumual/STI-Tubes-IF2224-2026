#pragma once

#include <any>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include "../ast/ASTVisitor.hpp"
#include "../ast/ASTNodes.hpp"
#include "../semanticanalyzer/SemanticAnalyzer.hpp"
#include "../symboltable/SymbolTable.hpp"
#include "Instruction.hpp"

class IntermediateCodeGenerator : public ASTVisitor {
    private:
        SymbolTable& symbolTable_;
        SemanticAnalyzer& semanticAnalyzer_;
        std::vector<Instruction> code_;
        static constexpr int FRAME_HEADER_SIZE = 3;
        std::unordered_map<int, int> relativeOffsetMap_;

        struct PendingCallPatch {
            int instructionIndex;
            int symbolIndex;
            std::string calleeName;
        };

        int currentLexicalLevel_ = 0;
        std::vector<std::string> currentFunctionNames_;
        std::vector<int> currentFunctionReturnSlots_;
        std::vector<PendingCallPatch> pendingCallPatches_;
        std::map<int, int> subprogramEntryLineBySymbolIndex_;

        int emit(OpCode op, int level, int operand);
        int emitLit(int value);
        int emitLod(int level, int address);
        int emitSto(int level, int address);
        int emitLda(int level, int address);
        int emitLdi();
        int emitSti();
        int emitChk(int low, int high);
        int emitCal(int level, int targetLine);
        int emitInt(int memorySize);
        int emitJmp(int targetLine);
        int emitJpc(int targetLine);
        int emitOpr(OprCode opr);
        int emitRet(int parameterCount = 0, bool returnsValue = false);

        void patchOperand(int instructionIndex, int newOperand);
        void patchPendingCalls();

        int currentLine() const;

        int getRuntimeAddress(ASTVariableExpressionNode* node) const;
        int getRuntimeLevel(ASTVariableExpressionNode* node) const;
        int getLevelDifference(int declarationLevel) const;
        int getBaseRuntimeAddress(ASTVariableExpressionNode* node) const;
        void emitVariableAddress(ASTVariableExpressionNode* node);

        int computeProgramMemorySize(ASTProgramNode* node) const;
        int computeDeclarationsMemorySize(ASTDeclarationsNode* node) const;
        int computeVariableMemorySize(ASTVarDeclarationNode* node) const;

        bool isBuiltinProcedure(const std::string& name) const;
        bool isWriteProcedure(const std::string& name) const;
        bool isWritelnProcedure(const std::string& name) const;
        bool isFunctionReturnTarget(ASTVariableExpressionNode* node) const;

        OprCode mapUnaryOperatorToOpr(const std::string& op) const;
        OprCode mapBinaryOperatorToOpr(const std::string& op) const;

        int literalToInt(const ASTLiteralExpressionNode* node) const;

        bool constantExpressionToInt(ASTExpressionNode* expression, int& outValue) const;

        int computeStaticComponentOffset(ASTVariableExpressionNode* node, ASTTypeNode* baseTypeNode) const;

        int computeRecordFieldOffset(ASTRecordTypeNode* recordTypeNode, const std::string& fieldName, ASTTypeNode*& fieldTypeNode) const;

        int emitCallToSubprogram(ASTCallExpressionNode* callExpr);

    public:
        explicit IntermediateCodeGenerator(SymbolTable& symbolTable, SemanticAnalyzer& semanticAnalyzer);

        std::vector<Instruction> generate(ASTProgramNode* root);

        const std::vector<Instruction>& getCode() const;

        std::string dumpCode() const;

        std::any visitProgramNode(ASTProgramNode* node) override;

        std::any visitDeclarationsNode(ASTDeclarationsNode* node) override;
        std::any visitDeclarationNode(ASTDeclarationNode* node) override;
        std::any visitConstDeclarationNode(ASTConstDeclarationNode* node) override;
        std::any visitTypeDeclarationNode(ASTTypeDeclarationNode* node) override;
        std::any visitVarDeclarationNode(ASTVarDeclarationNode* node) override;
        std::any visitSubprogramDeclarationNode(ASTSubprogramDeclarationNode* node) override;
        std::any visitProcedureDeclarationNode(ASTProcedureDeclarationNode* node) override;
        std::any visitFunctionDeclarationNode(ASTFunctionDeclarationNode* node) override;

        std::any visitTypeNode(ASTTypeNode* node) override;
        std::any visitPrimitiveType(ASTPrimitiveType* node) override;
        std::any visitNamedTypeNode(ASTNamedTypeNode* node) override;
        std::any visitRangeType(ASTRangeType* node) override;
        std::any visitArrayTypeNode(ASTArrayTypeNode* node) override;
        std::any visitEnumeratedTypeNode(ASTEnumeratedTypeNode* node) override;
        std::any visitRecordTypeNode(ASTRecordTypeNode* node) override;

        std::any visitStatementNode(ASTStatementNode* node) override;
        std::any visitEmptyStatementNode(ASTEmptyStatementNode* node) override;
        std::any visitBlockStatementNode(ASTBlockStatementNode* node) override;
        std::any visitAssignmentStatementNode(ASTAssignmentStatementNode* node) override;
        std::any visitIfStatementNode(ASTIfStatementNode* node) override;
        std::any visitWhileStatementNode(ASTWhileStatementNode* node) override;
        std::any visitRepeatStatementNode(ASTRepeatStatementNode* node) override;
        std::any visitForStatementNode(ASTForStatementNode* node) override;
        std::any visitCaseStatementNode(ASTCaseStatementNode* node) override;
        std::any visitCallStatementNode(ASTCallStatementNode* node) override;

        std::any visitExpressionNode(ASTExpressionNode* node) override;
        std::any visitLiteralExpressionNode(ASTLiteralExpressionNode* node) override;
        std::any visitVariableExpressionNode(ASTVariableExpressionNode* node) override;
        std::any visitUnaryExpressionNode(ASTUnaryExpressionNode* node) override;
        std::any visitBinaryExpressionNode(ASTBinaryExpressionNode* node) override;
        std::any visitCallExpressionNode(ASTCallExpressionNode* node) override;
};