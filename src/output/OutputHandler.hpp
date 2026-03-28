#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "../lexer/Token.hpp"

void writeToFile(const std::string& filename, const std::vector<Token>& tokens);