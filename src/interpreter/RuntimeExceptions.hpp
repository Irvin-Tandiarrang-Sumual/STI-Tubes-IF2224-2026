#pragma once
#include <stdexcept>
#include <string>

// Kerentanan Stack
class StackOverflowException : public std::runtime_error {
    public:
    StackOverflowException(const std::string& msg = "Runtime Error: Stack Overflow! (Infinite recursion atau batas memori terlampaui)") 
    : std::runtime_error(msg) {}
};

class StackUnderflowException : public std::runtime_error {
    public:
    StackUnderflowException(const std::string& msg = "Runtime Error: Stack Underflow! (Mencoba Pop dari stack yang kosong)") 
    : std::runtime_error(msg) {}
};

class StackCorruptionException : public std::runtime_error {
public:
    StackCorruptionException(
        const std::string& msg = "Runtime Error: Stack Corruption! (Struktur stack frame tidak valid)"
    ) : std::runtime_error(msg) {}
};

// Kerentanan Memori
class MemoryAccessException : public std::runtime_error {
public:
    MemoryAccessException(const std::string& msg = "Runtime Error: Out-of-Bounds Variable Access! (Mencoba mengakses indeks memori terlarang)") 
        : std::runtime_error(msg) {}
};

// Kerentanan Aritmatika
class NumericalOverflowException : public std::runtime_error {
    public:
    NumericalOverflowException(const std::string& msg = "Runtime Error: Numerical Overflow/Underflow! (Batas kapasitas angka terlampaui)") 
    : std::runtime_error(msg) {}
};

// Kerentanan Alur
class InvalidJumpTargetException : public std::runtime_error {
public:
    InvalidJumpTargetException(const std::string& msg = "Runtime Error: Invalid Jump Target! (Target lompatan GOTO/JMP tidak sah)") 
        : std::runtime_error(msg) {}
};