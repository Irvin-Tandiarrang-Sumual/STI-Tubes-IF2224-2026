#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include "Entries.hpp"
#include "DataType.hpp"
#include "Table.hpp"


class SymbolTable {
    private:
        Table<IdentifierTableEntry> tab_;
        Table<ArrayTableEntry>      atab_;
        Table<BlockTableEntry>      btab_;

        int currentLevel_ = 0;
        int currentBlockIdx_ = 0;
        int currentAddressOffset_ = 0;

        std::vector<int> blockHistory_;
        std::vector<int> offsetHistory_;

    public:
        SymbolTable();
        // scope management
        void enterBlock();

        void exitBlock();

        // regist symbol

        int insertVariable(const std::string& name, DataType type, int ref = 0);


        int lookup(const std::string& name);

        IdentifierTableEntry& getIdentifier(int index);
};