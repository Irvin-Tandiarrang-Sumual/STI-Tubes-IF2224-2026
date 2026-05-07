CXX = g++
CXXFLAGS = -Wall -std=c++17

# output
TARGET = main

# source files
SRC = $(wildcard *.cpp) \
      $(wildcard lexer/*.cpp) \
      $(wildcard reader/*.cpp) \
      $(wildcard writer/*.cpp) \
      $(wildcard cst/*.cpp) \
      $(wildcard parser/*.cpp) \
      $(wildcard parser/rules/*.cpp)

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)