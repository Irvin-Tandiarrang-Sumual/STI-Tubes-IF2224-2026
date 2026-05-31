#include "RuntimeStack.hpp"
#include <iostream>

RuntimeStack::RuntimeStack() : basePtr_(0) {}

void RuntimeStack::clear() { stack_.clear(); basePtr_ = 0; }

void RuntimeStack::allocate(int size) { stack_.resize(basePtr_ + size, 0); }

void RuntimeStack::push(int value) { stack_.push_back(value); }

int RuntimeStack::pop() { 
    int val = stack_.back(); 
    stack_.pop_back(); 
    return val; 
}

// Menelusuri Static Link untuk menemukan Base Pointer dari lexical parent
int RuntimeStack::resolveBase(int level) const {
    int currentBase = basePtr_;
    while (level > 0) {
        currentBase = stack_[currentBase]; // Index 0 dari frame header adalah Static Link
        level--;
    }
    return currentBase;
}

// Store & Load menggunakan base pointer sesuai level leksikal
void RuntimeStack::store(int level, int offset) {
    int val = pop();
    int targetBase = resolveBase(level);
    stack_[targetBase + offset] = val;
}

void RuntimeStack::load(int level, int offset) {
    int targetBase = resolveBase(level);
    push(stack_[targetBase + offset]);
}

// Helper Frame
int RuntimeStack::getBasePtr() const { return basePtr_; }
void RuntimeStack::setBasePtr(int bp) { basePtr_ = bp; }
int RuntimeStack::get(int index) const { return stack_[index]; }
void RuntimeStack::setSize(int newSize) { stack_.resize(newSize); }
int RuntimeStack::getSize() const { return stack_.size(); }

void RuntimeStack::printTrace() const {
    std::cout << "Stack: [";
    for (size_t i = 0; i < stack_.size(); ++i) {
        std::cout << stack_[i];
        if (i < stack_.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}