#include "../lexer/Token.hpp"
#include <vector>
#include <variant>
#include <memory>
#include "../parser/Variable.hpp"

class CSTNodes {
    private:
        bool isTerminal_; // artinya token
        std::vector<std::unique_ptr<CSTNodes>> children_; // kalo isTerminal true empty
        std::variant<Token, Variable> value_;

    public:
        CSTNodes(Token token);
        CSTNodes(Variable variable);
        void addChild(std::unique_ptr<CSTNodes> newChild);
        bool isTerminal() const;
        Variable getVariable() const;
        const Token& getToken() const;
        ~CSTNodes();
};