#include <iostream>
#include <vector>
#include <string>
#include <sstream>

template <typename T>
class Table {
    private:
        std::vector<T> entries_;
    public:
        void insert(const T& entry);
        T& get(int index);
        std::string toString(const std::string& tableName) const;
};