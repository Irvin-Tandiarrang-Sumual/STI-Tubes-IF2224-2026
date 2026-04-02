#include "OutputHandler.hpp"

std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeMap = {
        {TokenType::intcon, "intcon"}, {TokenType::realcon, "realcon"}, {TokenType::charcon, "charcon"}, {TokenType::string, "string"}, {TokenType::notsy, "notsy"}, {TokenType::plus, "plus"}, {TokenType::minus, "minus"}, {TokenType::times, "times"}, {TokenType::idiv, "idiv"}, {TokenType::rdiv, "rdiv"}, {TokenType::imod, "imod"}, {TokenType::andsy, "andsy"}, {TokenType::orsy, "orsy"}, {TokenType::eql, "eql"}, {TokenType::neq, "neq"}, {TokenType::gtr, "gtr"}, {TokenType::geq, "geq"}, {TokenType::lss, "lss"}, {TokenType::leq, "leq"}, {TokenType::lparent, "lparent"}, {TokenType::rparent, "rparent"}, {TokenType::lbrack, "lbrack"}, {TokenType::rbrack, "rbrack"}, {TokenType::comma, "comma"}, {TokenType::semicolon, "semicolon"}, {TokenType::period, "period"}, {TokenType::colon, "colon"}, {TokenType::becomes, "becomes"}, {TokenType::constsy, "constsy"}, {TokenType::typesy, "typesy"}, {TokenType::varsy, "varsy"}, {TokenType::functionsy, "functionsy"}, {TokenType::proceduresy, "proceduresy"}, {TokenType::arraysy, "arraysy"}, {TokenType::recordsy, "recordsy"}, {TokenType::programsy, "programsy"}, {TokenType::ident, "ident"}, {TokenType::beginsy, "beginsy"}, {TokenType::ifsy, "ifsy"}, {TokenType::casesy, "casesy"}, {TokenType::repeatsy, "repeatsy"}, {TokenType::whilesy, "whilesy"}, {TokenType::forsy, "forsy"}, {TokenType::endsy, "endsy"}, {TokenType::elsesy, "elsesy"}, {TokenType::untilsy, "untilsy"}, {TokenType::ofsy, "ofsy"}, {TokenType::dosy, "dosy"}, {TokenType::tosy, "tosy"}, {TokenType::downtosy, "downtosy"}, {TokenType::thensy, "thensy"}, {TokenType::comment, "comment"}
    };

    auto typeStr = typeMap.find(type);
    return (typeStr != typeMap.end()) ? typeStr->second : "??";
}

void writeToFile(const std::string& filename, const std::vector<Token>& tokens) {
    std::ofstream fOut(filename);
    if (!fOut.is_open()) return;

    for (const auto& token : tokens) {
        std::string typeStr = tokenTypeToString(token.type);

        if (token.type == TokenType::intcon || token.type == TokenType::realcon || token.type == TokenType::charcon || token.type == TokenType::string || token.type == TokenType::ident || token.type == TokenType::comment) {
            fOut << typeStr << " (";

            std::visit([&fOut](auto&& arg) {
                fOut << arg;
            }, token.value);

            fOut << ")\n";

        } else {
            fOut << typeStr << "\n";
        }
    }
    fOut.close();
}