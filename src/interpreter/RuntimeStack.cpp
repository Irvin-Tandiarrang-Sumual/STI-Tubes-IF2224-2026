#include "RuntimeStack.hpp"
#include <iostream>

namespace {
    int requireIntValue(const std::variant<int, double, char, std::string>& value, const std::string& context) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        }
        if (std::holds_alternative<char>(value)) {
            return static_cast<int>(std::get<char>(value));
        }
        throw MemoryAccessException("Runtime Error: " + context + " harus berupa nilai ordinal/integer.");
    }
}

RuntimeStack::RuntimeStack() : basePtr_(0) {}

void RuntimeStack::clear() { stack_.clear(); basePtr_ = 0; }

void RuntimeStack::allocate(int size) {
    if (basePtr_ + size > static_cast<int>(MAX_STACK_SIZE)) {
        throw StackOverflowException();
    }
    stack_.resize(basePtr_ + size, 0); 
}

void RuntimeStack::push(std::variant<int, double, char, std::string> value) {
    if (static_cast<int>(stack_.size()) >= static_cast<int>(MAX_STACK_SIZE)) {
        throw StackOverflowException();
    }
    stack_.push_back(value);
}

std::variant<int, double, char, std::string> RuntimeStack::pop() {
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

void RuntimeStack::pushAddress(int level, int offset) {
    int targetBase = resolveBase(level);
    int targetAddress = targetBase + offset;

    validateIndex(targetAddress, "LDA");
    push(targetAddress);
}

void RuntimeStack::indirectLoad() {
    auto addressValue = pop();
    int address = requireIntValue(addressValue, "Alamat indirect load");

    validateIndex(address, "LDI");
    push(stack_[address]);
}

void RuntimeStack::indirectStore() {
    auto value = pop();
    auto addressValue = pop();

    int address = requireIntValue(addressValue, "Alamat indirect store");

    validateIndex(address, "STI");
    stack_[address] = value;
}

void RuntimeStack::checkBounds(int low, int high) {
    auto indexValue = pop();
    int index = requireIntValue(indexValue, "Index array");

    if (index < low || index > high) {
        throw MemoryAccessException("Runtime Error: Array index out of bounds. Index " + std::to_string(index) + " tidak berada dalam range [" + std::to_string(low) + ".." + std::to_string(high) + "].");
    }

    push(index);
}

int RuntimeStack::getBasePtr() const { return basePtr_; }

void RuntimeStack::setBasePtr(int bp) { basePtr_ = bp; }

std::variant<int, double, char, std::string> RuntimeStack::get(int index) const { return stack_[index]; }

void RuntimeStack::setSize(int newSize) { stack_.resize(newSize); }

int RuntimeStack::getSize() const { return static_cast<int>(stack_.size()); }

void RuntimeStack::printTrace() const {
    std::cout << "Stack: [";
    for (int i = 0; i < static_cast<int>(stack_.size()); ++i) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                std::cout << "\"" << arg << "\""; 
            } else if constexpr (std::is_same_v<T, char>) {
                std::cout << "'" << arg << "'";
            } else {
                std::cout << arg;                 
            }
        }, stack_[i]);
        
        if (i < static_cast<int>(stack_.size()) - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void RuntimeStack::validateIndex(int index, const std::string& action) const {
    if (index < 0 || index >= static_cast<int>(stack_.size())) {
        throw MemoryAccessException("Runtime Error: " + action + " mengakses alamat memori invalid: " + std::to_string(index) + ". Ukuran stack saat ini: " + std::to_string(stack_.size()) + ".");
    }
}