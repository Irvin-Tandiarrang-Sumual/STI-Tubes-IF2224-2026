#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include "DataType.hpp"
struct IdentifierTableEntry {
    explicit IdentifierTableEntry(std::string name, int linkIndex, DataType type, int reference, bool normal, int level, int address, std::string obj = "")
        : name(std::move(name)), index(0), obj(std::move(obj)), linkIndex(linkIndex), type(type), reference(reference), normal(normal), level(level), address(address), typeName(""), isConstant(false) {}
    // Nama identifier (misalnya nama variabel, konstanta, tipe, prosedur, fungsi).
    std::string name;

    // Indeks dimulai dari 33 karena ada reserved words termasuk predefined identifiers.
    int index;

    // Kelas objek: konstanta, variabel, tipe, prosedur, fungsi, keyword, dll.
    std::string obj;

    
    // Pointer/indeks ke identifier sebelumnya dalam scope yang sama.
    // Digunakan untuk manajemen scope (linked list per blok).
    int linkIndex;

    // Tipe dasar dari identifier, misalnya: integer, boolean, char,
    // real, array, record, dll. Biasanya berupa kode numerik.
    DataType type;
    
    //Pointer/indeks ke tabel lain jika tipe adalah komposit (array/record).
    // Mengarah ke atab (array table) atau btab (record/procedure block)
    int reference;

    // Menandai apakah identifier adalah variabel normal (=1)
    // atau parameter by-reference (var parameter) (=0).
    bool normal;

    // Tingkat lexical level tempat identifier dideklarasikan
    // (0 = global, 1 = dalam prosedur, 2 = dalam prosedur di dalam prosedur, dst).
    int level;
            
    // Makna tergantung jenis objek: offset variabel di stack frame,
    //nilai konstanta, offset field record, alamat prosedur, atau ukuran/penanda lain.
    int address;

    // kasus struct
    std::string typeName;

    // Ngecek sebuah variabel itu constant atau nggak, kalau constant kan berarti gaboleh diubah
    bool isConstant; 
    std::vector<DataType> parameterTypes;

    // to string
    static std::string dataTypeToString(DataType dt) {
        switch (dt) {
            case DataType::REAL: return "real";
            case DataType::INTEGER: return "integer";
            case DataType::CHAR: return "char";
            case DataType::BOOLEAN: return "boolean";
            case DataType::STRING: return "string";
            case DataType::RANGE: return "range";
            case DataType::ENUMERATED: return "enumerated";
            case DataType::ARRAY: return "array";
            case DataType::RECORD: return "record";
            case DataType::VOID: return "void";
            default: return "unknown";
        }
    }
    static std::vector<std::size_t> getColumnWidths(const std::vector<IdentifierTableEntry>& entries) {
        std::vector<std::size_t> widths = {3, 2, 3, 4, 3, 3, 3, 3, 4};

        auto grow = [](std::size_t& width, const std::string& text) {
            width = std::max(width, text.size());
        };

        grow(widths[0], "idx");
        grow(widths[1], "id");
        grow(widths[2], "obj");
        grow(widths[3], "type");
        grow(widths[4], "ref");
        grow(widths[5], "nrm");
        grow(widths[6], "lev");
        grow(widths[7], "adr");
        grow(widths[8], "link");

        for (const auto& entry : entries) {
            grow(widths[0], std::to_string(entry.index));
            grow(widths[1], entry.name);
            grow(widths[2], entry.obj);
            grow(widths[3], dataTypeToString(entry.type));
            grow(widths[4], std::to_string(entry.reference));
            grow(widths[5], std::to_string(entry.normal ? 1 : 0));
            grow(widths[6], std::to_string(entry.level));
            grow(widths[7], std::to_string(entry.address));
            grow(widths[8], std::to_string(entry.linkIndex));
        }

        return widths;
    }

    static std::string getHeader(const std::vector<std::size_t>& widths) {
        std::stringstream ss;
        ss << std::left 
            << std::setw(static_cast<int>(widths[0]) + 1) << "idx"
            << std::setw(static_cast<int>(widths[1]) + 1) << "id"
            << std::setw(static_cast<int>(widths[2]) + 1) << "obj"
            << std::setw(static_cast<int>(widths[3]) + 1) << "type"
            << std::setw(static_cast<int>(widths[4]) + 1) << "ref"
            << std::setw(static_cast<int>(widths[5]) + 1) << "nrm"
            << std::setw(static_cast<int>(widths[6]) + 1) << "lev"
            << std::setw(static_cast<int>(widths[7]) + 1) << "adr"
            << std::setw(static_cast<int>(widths[8]) + 1) << "link";
        return ss.str();
    }

    static std::string getHeader() {
        return getHeader(getColumnWidths({}));
    }

    std::string toString(const std::vector<std::size_t>& widths) const {
        std::stringstream ss;
        ss << std::left 
            << std::setw(static_cast<int>(widths[0]) + 1) << index
            << std::setw(static_cast<int>(widths[1]) + 1) << name
            << std::setw(static_cast<int>(widths[2]) + 1) << obj
            << std::setw(static_cast<int>(widths[3]) + 1) << dataTypeToString(type)
            << std::setw(static_cast<int>(widths[4]) + 1) << reference
            << std::setw(static_cast<int>(widths[5]) + 1) << (normal ? 1 : 0)
            << std::setw(static_cast<int>(widths[6]) + 1) << level
            << std::setw(static_cast<int>(widths[7]) + 1) << address
            << std::setw(static_cast<int>(widths[8]) + 1) << linkIndex;
        return ss.str();
    }

    std::string toString() const {
        return toString(getColumnWidths({*this}));
    }
};

struct ArrayTableEntry{
        explicit ArrayTableEntry(int arrayIndex, DataType indexType, DataType elementType, int compositeTypeReference,
                                                        int low, int high, int elementSize, int size)
                : arrayIndex(arrayIndex), indexType(indexType), elementType(elementType), compositeTypeReference(compositeTypeReference),
                    low(low), high(high), elementSize(elementSize), size(size), typeName("") {}
    // Indeks entri array
    int arrayIndex;

    // Tipe indeks array (misalnya integer).
    // Berupa kode tipe dari tabel tab.
    DataType indexType;

    // Tipe elemen array (misalnya integer).
    // Berupa kode tipe dari tabel tab.
    DataType elementType;

    // Pointer/indeks ke detail tipe elemen jika elemen
    // adalah tipe komposit (misalnya array dalam array, atau record).
    // Mengarah ke atab atau btab.
    int compositeTypeReference;
    
    // Batas bawah indeks array (misalnya 1 pada array[1..10]
    // atau 0 pada array[0..15]).
    int low;
    
    // batas atas indeks array
    int high;
    
    // ukuran satu elemen array (dalam byte/unit memori)
    int elementSize;

    // total ukuran array
    int size;

    // kasus struct
    std::string typeName;

    // to string

    static std::vector<std::size_t> getColumnWidths(const std::vector<ArrayTableEntry>& entries) {
        std::vector<std::size_t> widths = {3, 6, 5, 3, 3, 4, 4, 4};

        auto grow = [](std::size_t& width, const std::string& text) {
            width = std::max(width, text.size());
        };

        grow(widths[0], "idx");
        grow(widths[1], "inxtyp");
        grow(widths[2], "eltyp");
        grow(widths[3], "ref");
        grow(widths[4], "low");
        grow(widths[5], "high");
        grow(widths[6], "elsz");
        grow(widths[7], "size");

        for (const auto& entry : entries) {
            grow(widths[0], std::to_string(entry.arrayIndex));
            grow(widths[1], IdentifierTableEntry::dataTypeToString(entry.indexType));
            grow(widths[2], IdentifierTableEntry::dataTypeToString(entry.elementType));
            grow(widths[3], std::to_string(entry.compositeTypeReference));
            grow(widths[4], std::to_string(entry.low));
            grow(widths[5], std::to_string(entry.high));
            grow(widths[6], std::to_string(entry.elementSize));
            grow(widths[7], std::to_string(entry.size));
        }

        return widths;
    }

    static std::string getHeader(const std::vector<std::size_t>& widths) {
        std::stringstream ss;
        ss << std::left 
            << std::setw(static_cast<int>(widths[0]) + 1) << "idx"
            << std::setw(static_cast<int>(widths[1]) + 1) << "inxtyp"
            << std::setw(static_cast<int>(widths[2]) + 1) << "eltyp"
            << std::setw(static_cast<int>(widths[3]) + 1) << "ref"
            << std::setw(static_cast<int>(widths[4]) + 1) << "low"
            << std::setw(static_cast<int>(widths[5]) + 1) << "high"
            << std::setw(static_cast<int>(widths[6]) + 1) << "elsz"
            << std::setw(static_cast<int>(widths[7]) + 1) << "size";
        return ss.str();
    }

    static std::string getHeader() {
        return getHeader(getColumnWidths({}));
    }

    std::string toString(const std::vector<std::size_t>& widths) const {
        std::stringstream ss;
        ss << std::left 
            << std::setw(static_cast<int>(widths[0]) + 1) << arrayIndex
            << std::setw(static_cast<int>(widths[1]) + 1) << IdentifierTableEntry::dataTypeToString(indexType)
            << std::setw(static_cast<int>(widths[2]) + 1) << IdentifierTableEntry::dataTypeToString(elementType)
            << std::setw(static_cast<int>(widths[3]) + 1) << compositeTypeReference
            << std::setw(static_cast<int>(widths[4]) + 1) << low
            << std::setw(static_cast<int>(widths[5]) + 1) << high
            << std::setw(static_cast<int>(widths[6]) + 1) << elementSize
            << std::setw(static_cast<int>(widths[7]) + 1) << size;
        return ss.str();
    }
    std::string toString() const {
        return toString(getColumnWidths({*this}));
    }
};

struct BlockTableEntry {
    explicit BlockTableEntry(int blockIndex, int last, int latestParameter, int parentBlock, int parameterSize, int variableSize)
        : blockIndex(blockIndex), last(last), latestParameter(latestParameter), parentBlock(parentBlock), parameterSize(parameterSize), variableSize(variableSize) {}
    // Indeks entri block (setiap block mewakili
    // prosedur, fungsi, atau record type definition).
    int blockIndex;

    // Pointer/indeks ke identifier terakhir yang dideklarasikan
    // di dalam block tersebut (menghubungkan field record,
    // parameter, atau variabel lokal).
    int last;

    // Pointer/indeks ke parameter terakhir dari prosedur/fungsi pada block tersebut. Jika block adalah record, nilainya 0.
    int latestParameter;

    int parentBlock;

    // Total ukuran parameter block  (dalam byte/unit memori).
    int parameterSize;

    // Total ukuran variabel lokal block (dalam byte/unit memori)
    int variableSize;

    static std::vector<std::size_t> getColumnWidths(const std::vector<BlockTableEntry>& entries) {
        std::vector<std::size_t> widths = {3, 4, 6, 5, 5};

        auto grow = [](std::size_t& width, const std::string& text) {
            width = std::max(width, text.size());
        };

        grow(widths[0], "idx");
        grow(widths[1], "last");
        grow(widths[2], "lparam");
        grow(widths[3], "psize");
        grow(widths[4], "vsize");

        for (const auto& entry : entries) {
            grow(widths[0], std::to_string(entry.blockIndex));
            grow(widths[1], std::to_string(entry.last));
            grow(widths[2], std::to_string(entry.latestParameter));
            grow(widths[3], std::to_string(entry.parameterSize));
            grow(widths[4], std::to_string(entry.variableSize));
        }

        return widths;
    }

    static std::string getHeader(const std::vector<std::size_t>& widths) {
        std::stringstream ss;
        ss << std::left 
            << std::setw(static_cast<int>(widths[0]) + 1) << "idx"
            << std::setw(static_cast<int>(widths[1]) + 1) << "last"
            << std::setw(static_cast<int>(widths[2]) + 1) << "lparam"
            << std::setw(static_cast<int>(widths[3]) + 1) << "psize"
            << std::setw(static_cast<int>(widths[4]) + 1) << "vsize";
        return ss.str();
    }

    static std::string getHeader() {
        return getHeader(getColumnWidths({}));
    }

    // 2. Fungsi untuk mencetak baris data Block
    std::string toString(const std::vector<std::size_t>& widths) const {
        std::stringstream ss;
        ss << std::left 
            << std::setw(static_cast<int>(widths[0]) + 1) << blockIndex
            << std::setw(static_cast<int>(widths[1]) + 1) << last
            << std::setw(static_cast<int>(widths[2]) + 1) << latestParameter
            << std::setw(static_cast<int>(widths[3]) + 1) << parameterSize
            << std::setw(static_cast<int>(widths[4]) + 1) << variableSize;
        return ss.str();
    }

    std::string toString() const {
        return toString(getColumnWidths({*this}));
    }
};