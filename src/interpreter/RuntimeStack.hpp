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
    void allocate(int size);           // Untuk OpCode::INT
    void push(int value);              // Untuk OpCode::LIT
    int pop();                         // Mengambil nilai teratas
    void store(int level, int offset); // Untuk OpCode::STO
    void load(int level, int offset);  // Untuk OpCode::LOD
    
    // Fungsi Helper Stack Frame
    int getBasePtr() const;
    void setBasePtr(int bp);
    int resolveBase(int level) const;
    int get(int index) const;
    void setSize(int newSize);
    int getSize() const;
    
    // Tracing Stack
    void printTrace() const;
};