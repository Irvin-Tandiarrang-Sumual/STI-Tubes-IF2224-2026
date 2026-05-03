#include "../lexer/Token.hpp"
#include <vector>
#include <variant>
#include <memory>
#include "../parser/NonTerminal.hpp"

class CSTNodes {
    private:
        bool isTerminal_; // artinya token
        std::vector<std::unique_ptr<CSTNodes>> children_; // kalo isTerminal true empty
        std::variant<Token, NonTerminal> value_;
        CodeLocation location_;

    public:
        CSTNodes(Token token, CodeLocation location);
        CSTNodes(NonTerminal nonTerminal, CodeLocation location);
        void addChild(std::unique_ptr<CSTNodes> newChild);
        bool isTerminal() const;
        NonTerminal getNonTerminal() const;
        const Token& getToken() const;
        const CodeLocation& getLocation() const;
        ~CSTNodes();
};