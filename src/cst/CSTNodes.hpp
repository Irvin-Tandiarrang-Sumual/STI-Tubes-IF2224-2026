#pragma once

#include "../lexer/Token.hpp"
#include <vector>
#include <variant>
#include <memory>
#include "../parser/NonTerminal.hpp"

class CSTNodes {
    private:
        bool isTerminal_; // artinya token
        std::vector<CSTNodes*> children_; // kalo isTerminal true empty
        std::variant<Token, NonTerminal> value_;
        CodeLocation location_;
        bool isError_; // khusus error


    public:
        CSTNodes(NonTerminal nonTerminal, CodeLocation location);
        CSTNodes(Token token);
        CSTNodes(std::string message, CodeLocation location);
        bool isError() const;
        void addChild(CSTNodes* newChild);
        bool isTerminal() const;
        const NonTerminal& getNonTerminal() const;
        const Token& getToken() const;
        const CodeLocation& getLocation() const;
        const std::vector<CSTNodes*>& getChildren() const;
        const std::string toString() const;
        ~CSTNodes();
        
};