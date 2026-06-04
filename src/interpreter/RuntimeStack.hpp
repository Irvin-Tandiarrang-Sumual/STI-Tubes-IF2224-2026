#pragma once
#include <vector>
#include <string>
#include <variant>
#include "RuntimeExceptions.hpp"

class RuntimeStack {
private:
    std::vector<std::variant<int, double, char, std::string>> stack_;
    int basePtr_;
    const size_t MAX_STACK_SIZE = 10000; // Batas aman maksimum ukuran Stack

public:
    RuntimeStack();
    void clear();
    
    // OpCode Execution
    void allocate(int size);           
    void push(std::variant<int, double, char, std::string> value);
    std::variant<int, double, char, std::string> pop();         
    void store(int level, int offset); 
    void load(int level, int offset);  
    
    // Fungsi Helper Stack Frame
    int getBasePtr() const;
    void setBasePtr(int bp);
    int resolveBase(int level) const;
    std::variant<int, double, char, std::string> get(int index) const;
    void setSize(int newSize);
    int getSize() const;
    
    // Tracing Stack
    void printTrace() const;
};