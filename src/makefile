CXX = g++
CXXFLAGS = -Wall -std=c++17

# output
TARGET = main

# source files
SRC = main.cpp \
      Compiler.cpp \
      lexer/Lexer.cpp lexer/Token.cpp \
      reader/Reader.cpp reader/CodeLocation.cpp \
      writer/Writer.cpp \
      cst/CSTNodes.cpp \
      parser/Parser.cpp \
      parser/rules/ProgramStructure.cpp \
      parser/rules/Declarations.cpp \
      parser/rules/Types.cpp \
      parser/rules/Parameters.cpp \
      parser/rules/Statements.cpp \
      parser/rules/ExecutionStatements.cpp \
      parser/rules/ControlFlowStatements.cpp \
      parser/rules/Variables.cpp \
      parser/rules/Expressions.cpp \
      parser/rules/Operators.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)