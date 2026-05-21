#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

template <typename T>
class Table {
    private:
        std::vector<T> entries_;
    public:
        void insert(const T& entry) {
            entries_.push_back(entry);
        }

        T& get(int index) {
            return entries_.at(index);
        }
        std::string toString(const std::string& tableName) const {
            std::stringstream ss;
            ss << tableName << " :\n";
            
            ss << T::getHeader() << "\n";
            
            ss << std::string(60, '-') << "\n"; 
            
            for (size_t i = 0; i < entries_.size(); ++i) {
                ss << entries_[i].toString() << "\n";
            }
            return ss.str();
        }

        int size() {
            return entries_.size();
        }
};