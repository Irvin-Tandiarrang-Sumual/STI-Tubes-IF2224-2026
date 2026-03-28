#include "Writer.hpp"

Writer :: Writer (const std::string& filename, const std::vector<Token>& tokens)
    : filename(filename), tokens(tokens) {}

Writer :: ~Writer() {}

void Writer::writeToFile() const {
    std::ofstream fOut(filename);
    if (!fOut.is_open()) return;

    for (const auto& token : tokens) {
        std::string typeStr = tokenTypeToString(token.type);

        if (token.type == TokenType::intcon || token.type == TokenType::realcon
            || token.type == TokenType::charcon || token.type == TokenType::string
            || token.type == TokenType::ident || token.type == TokenType::comment) {
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