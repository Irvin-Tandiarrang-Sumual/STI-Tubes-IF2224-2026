#include "Table.hpp"

template <typename T>
void Table<T>::insert(const T& entry) {
    entries_.push_back(entry);
}

template <typename T>
T& Table<T>::get(int index) {
    return entries_.at(index);
}