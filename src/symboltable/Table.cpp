#include "Table.hpp"

template <typename T>
void Table<T>::insert(const T& entry) {
    entries_.push_back(entry);
}

template <typename T>
T& Table<T>::get(int index) {
    return entries_.at(index);
}
template <typename T>
std::string Table<T>::toString(const std::string& tableName) const {
    std::stringstream ss;
    ss << tableName << " :\n";
    
    ss << T::getHeader() << "\n";
    
    ss << std::string(60, '-') << "\n"; 
    
    for (size_t i = 0; i < entries_.size(); ++i) {
        ss << entries_[i].toString() << "\n";
    }
    return ss.str();
}