#include "Token.hpp"

#include <sstream>

std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeMap = {
        {TokenType::eof, "eof"},
        {TokenType::invalid_token, "UNKNOWN"},
        {TokenType::intcon, "intcon"},
        {TokenType::realcon, "realcon"},
        {TokenType::charcon, "charcon"},
        {TokenType::string, "string"},
        {TokenType::notsy, "notsy"},
        {TokenType::plus, "plus"},
        {TokenType::minus, "minus"},
        {TokenType::times, "times"},
        {TokenType::idiv, "idiv"},
        {TokenType::rdiv, "rdiv"},
        {TokenType::imod, "imod"},
        {TokenType::andsy, "andsy"},
        {TokenType::orsy, "orsy"},
        {TokenType::eql, "eql"},
        {TokenType::neq, "neq"},
        {TokenType::gtr, "gtr"},
        {TokenType::geq, "geq"},
        {TokenType::lss, "lss"},
        {TokenType::leq, "leq"},
        {TokenType::lparent, "lparent"},
        {TokenType::rparent, "rparent"},
        {TokenType::lbrack, "lbrack"},
        {TokenType::rbrack, "rbrack"},
        {TokenType::comma, "comma"},
        {TokenType::semicolon, "semicolon"},
        {TokenType::period, "period"},
        {TokenType::colon, "colon"},
        {TokenType::becomes, "becomes"},
        {TokenType::constsy, "constsy"},
        {TokenType::typesy, "typesy"},
        {TokenType::varsy, "varsy"},
        {TokenType::functionsy, "functionsy"},
        {TokenType::proceduresy, "proceduresy"},
        {TokenType::arraysy, "arraysy"},
        {TokenType::recordsy, "recordsy"},
        {TokenType::programsy, "programsy"},
        {TokenType::ident, "ident"},
        {TokenType::beginsy, "beginsy"},
        {TokenType::ifsy, "ifsy"},
        {TokenType::casesy, "casesy"},
        {TokenType::repeatsy, "repeatsy"},
        {TokenType::whilesy, "whilesy"},
        {TokenType::forsy, "forsy"},
        {TokenType::endsy, "endsy"},
        {TokenType::elsesy, "elsesy"},
        {TokenType::untilsy, "untilsy"},
        {TokenType::ofsy, "ofsy"},
        {TokenType::dosy, "dosy"},
        {TokenType::tosy, "tosy"},
        {TokenType::downtosy, "downtosy"},
        {TokenType::thensy, "thensy"},
        {TokenType::comment, "comment"}
    };

    auto it = typeMap.find(type);
    return (it != typeMap.end()) ? it->second : "UNKNOWN";
}

std::string tokenValueToString(const Token &token) {
    return std::visit([](const auto &arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else {
            std::ostringstream oss;
            oss << arg;
            return oss.str();
        }
    }, token.value);
}

bool tokenNeedsValue(TokenType type) {
    return type == TokenType::intcon || type == TokenType::realcon || type == TokenType::charcon ||
           type == TokenType::string || type == TokenType::ident || type == TokenType::comment ||
           type == TokenType::invalid_token;
}
