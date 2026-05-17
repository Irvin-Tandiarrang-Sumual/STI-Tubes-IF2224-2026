#include <iostream>
#include <vector>
template <typename T>
class Table {
    private:
        std::vector<T> entries_;
    public:
        void insert(const T& entry);
        T& get(int index);
};