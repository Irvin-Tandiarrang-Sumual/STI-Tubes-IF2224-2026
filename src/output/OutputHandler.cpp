#include "OutputHandler.hpp"

#include "Writer.hpp"

void writeToFile(const std::string &filename, const std::vector<Token> &tokens) {
    Writer writer(filename, tokens);
    writer.writeToFile();
}
