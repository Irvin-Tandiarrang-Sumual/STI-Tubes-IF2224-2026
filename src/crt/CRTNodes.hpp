#include "../lexer/Token.hpp"
#include <vector>
#include <variant>
#include <memory>
#include "../parser/Variable.hpp"

class CRTNodes {
    private:
        bool isTerminal_; // artinya token
        std::vector<std::unique_ptr<CRTNodes>> children_; // kalo isTerminal true empty
        std::variant<Token, Variable> value_;

    public:
        CRTNodes(Token token);
        CRTNodes(Variable variable);
        void addChild(std::unique_ptr<CRTNodes> newChild);
        bool isTerminal() const;
        Variable getVariable() const;
        const Token& getToken() const;
        ~CRTNodes();
};