#include "RuntimeStack.hpp"
#include <iostream>

RuntimeStack::RuntimeStack() : basePtr_(0) {}

void RuntimeStack::clear() { stack_.clear(); basePtr_ = 0; }

void RuntimeStack::allocate(int size) {
    if (basePtr_ + size > static_cast<int>(MAX_STACK_SIZE)) {
        throw StackOverflowException();
    }
    stack_.resize(basePtr_ + size, 0); 
}

void RuntimeStack::push(std::variant<int, std::string> value) {
    if (static_cast<int>(stack_.size()) >= static_cast<int>(MAX_STACK_SIZE)) {
        throw StackOverflowException();
    }
    stack_.push_back(value);
}

std::variant<int, std::string> RuntimeStack::pop() { 
    if (stack_.empty()) {
        throw StackUnderflowException();
    }
    auto val = stack_.back(); 
    stack_.pop_back(); 
    return val;
}

int RuntimeStack::resolveBase(int level) const {
    int currentBase = basePtr_;
    while (level > 0) {
        currentBase = std::get<int>(stack_[currentBase]); 
        level--;
    }
    return currentBase;
}

void RuntimeStack::store(int level, int offset) {
    auto val = pop();
    int targetBase = resolveBase(level);
    int targetAddress = targetBase + offset;
    
    if (targetAddress < 0 || targetAddress >= static_cast<int>(stack_.size())) {
        throw MemoryAccessException("Runtime Error: Invalid STORE! Alamat memori " + std::to_string(targetAddress) + " terlarang.");
    }
    stack_[targetAddress] = val;
}

void RuntimeStack::load(int level, int offset) {
    int targetBase = resolveBase(level);
    int targetAddress = targetBase + offset;
    
    if (targetAddress < 0 || targetAddress >= static_cast<int>(stack_.size())) {
        throw MemoryAccessException("Runtime Error: Invalid LOAD! Alamat memori " + std::to_string(targetAddress) + " terlarang.");
    }
    push(stack_[targetAddress]);
}

int RuntimeStack::getBasePtr() const { return basePtr_; }
void RuntimeStack::setBasePtr(int bp) { basePtr_ = bp; }
std::variant<int, std::string> RuntimeStack::get(int index) const { return stack_[index]; }
void RuntimeStack::setSize(int newSize) { stack_.resize(newSize); }
int RuntimeStack::getSize() const { return static_cast<int>(stack_.size()); }

void RuntimeStack::printTrace() const {
    std::cout << "Stack: [";
    for (int i = 0; i < static_cast<int>(stack_.size()); ++i) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                std::cout << "\"" << arg << "\""; 
            } else {
                std::cout << arg;                 
            }
        }, stack_[i]);
        
        if (i < static_cast<int>(stack_.size()) - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}