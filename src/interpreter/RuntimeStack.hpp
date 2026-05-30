#pragma once
#include <vector>

class RuntimeStack {
private:
    std::vector<int> stack_;
    int basePtr_;

public:
    RuntimeStack();
    void clear();
    
    // OpCode Execution
    void allocate(int size);         // Untuk OpCode::INT
    void push(int value);            // Untuk OpCode::LIT
    int pop();                       // Mengambil nilai teratas
    void store(int offset);          // Untuk OpCode::STO
    void load(int offset);           // Untuk OpCode::LOD
    
    // Tracing Stack
    void printTrace() const;
};