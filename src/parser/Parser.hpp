#include <iostream>
#include <vector>
#include "../lexer/Token.hpp"
#include "../crt/CRTNodes.hpp"
#include <memory>

class Parser {
    private:
        const std::vector<Token>& tokens_;
        size_t currentPosition = 0;
    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser();
        
        // return root node aja
        std::unique_ptr<CRTNodes> parse();

        // lihat 1 ke depan
        const Token& peek() const;

        // maju 1
        const Token& advance();
};


