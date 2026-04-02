CXX = g++
CXXFLAGS = -Wall -std=c++17

# output
TARGET = main

# source files
SRC = main.cpp \
      Compiler.cpp \
      lexer/Lexer.cpp lexer/Token.cpp \
      reader/Reader.cpp reader/CodeLocation.cpp \
      writer/Writer.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)