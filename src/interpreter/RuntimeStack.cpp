#include "RuntimeStack.hpp"
#include <iostream>

RuntimeStack::RuntimeStack() : basePtr_(0) {}

void RuntimeStack::clear() { stack_.clear(); basePtr_ = 0; }

void RuntimeStack::allocate(int size) { stack_.resize(stack_.size() + size, 0); }

void RuntimeStack::push(int value) { stack_.push_back(value); }

int RuntimeStack::pop() { 
    int val = stack_.back(); 
    stack_.pop_back(); 
    return val; 
}

void RuntimeStack::store(int offset) {
    int val = pop();
    stack_[basePtr_ + offset] = val;
}

void RuntimeStack::load(int offset) {
    push(stack_[basePtr_ + offset]);
}

void RuntimeStack::printTrace() const {
    std::cout << "Stack: [";
    for (size_t i = 0; i < stack_.size(); ++i) {
        std::cout << stack_[i];
        if (i < stack_.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}