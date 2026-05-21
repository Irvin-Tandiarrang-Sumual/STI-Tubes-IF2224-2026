#include "SymbolTable.hpp"
SymbolTable::SymbolTable() {
    btab_.insert(BlockTableEntry(0, 0, 0, 0, 0));
}

void SymbolTable::enterBlock() {
    currentLevel_++;
    blockHistory_.push_back(currentBlockIdx_);
    offsetHistory_.push_back(currentAddressOffset_);

    // Bikin blok baru di btab
    int newBlockIdx = btab_.size();
    btab_.insert(BlockTableEntry(newBlockIdx, 0, 0, 0, 0));
    currentBlockIdx_ = newBlockIdx;
    
    // reset di blok skrg
    currentAddressOffset_ = 0;
}

void SymbolTable::exitBlock() {
    if (blockHistory_.empty()) {
        throw std::runtime_error("Symbol Table Error: Mencoba keluar dari global scope.");
    }
    
    // Update ukuran variabel lokal di btab sebelum keluar
    BlockTableEntry& currentBlock = btab_.get(currentBlockIdx_);
    currentBlock.variableSize = currentAddressOffset_;

    // pop state scope sblmnya
    currentLevel_--;
    currentBlockIdx_ = blockHistory_.back();
    currentAddressOffset_ = offsetHistory_.back();
    
    blockHistory_.pop_back();
    offsetHistory_.pop_back();
}

int SymbolTable::insertVariable(const std::string& name, DataType type, int ref) {
    BlockTableEntry& currentBlock = btab_.get(currentBlockIdx_);
    int previousLink = currentBlock.last;

    int newIdx = tab_.size();
    IdentifierTableEntry entry(
        name, 
        previousLink, // link idx ke current
        type, 
        ref, 
        true, // normal variable
        currentLevel_, 
        currentAddressOffset_
    );
    entry.index = newIdx;

    tab_.insert(entry);

    currentBlock.last = newIdx;

    // default 4 byte per variabel dasar
    currentAddressOffset_ += 4;

    return newIdx;
}

int SymbolTable::lookup(const std::string& name) {
    // naik
    int blockIdx = currentBlockIdx_;
    
    while (blockIdx >= 0) {
        BlockTableEntry& block = btab_.get(blockIdx);
        int currentIdIdx = block.last;
        
        while (currentIdIdx > 0) {
            IdentifierTableEntry& id = tab_.get(currentIdIdx);
            if (id.name == name) {
                return currentIdIdx;
            }
            currentIdIdx = id.linkIndex;
        }
        
        blockIdx--; 
    }
    
    return -1; // not found
}

IdentifierTableEntry& SymbolTable::getIdentifier(int index) {
    return tab_.get(index);
}

std::string SymbolTable::dumpTab() const {
    return tab_.toString("tab");
}

std::string SymbolTable::dumpBTab() const {
    return btab_.toString("btab");
}

std::string SymbolTable::dumpATab() const {
    return atab_.toString("atab");
}

std::string SymbolTable::dumpTables() const {
    std::string result;
    result += dumpTab();
    result += "\n";
    result += dumpBTab();
    result += "\n";
    result += dumpATab();
    return result;
}