#include <iostream>
#include <sstream>
#include <iomanip>
#include "DataType.hpp"
struct IdentifierTableEntry {
    explicit IdentifierTableEntry(std::string name, int linkIndex, DataType type, int reference, bool normal, int level, int address);
    // Nama identifier (misalnya nama variabel, konstanta, tipe, prosedur, fungsi).
    std::string name;

    // Indeks dimulai dari 33 karena ada reserved words termasuk predefined identifiers.
    int index;

    
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

    // to string
    static::std::string getHeader() {
        std::stringstream ss;
        ss << std::left 
            << std::setw(5)  << "idx"
            << std::setw(12) << "id"
            << std::setw(6)  << "type"
            << std::setw(6)  << "ref"
            << std::setw(6)  << "nrm"
            << std::setw(6)  << "lev"
            << std::setw(6)  << "adr"
            << std::setw(6)  << "link";
        return ss.str();
    }

    std::string toString() const {
        std::stringstream ss;
        ss << std::left 
            << std::setw(5)  << index
            << std::setw(12) << name
            << std::setw(6)  << static_cast<int>(type)
            << std::setw(6)  << reference
            << std::setw(6)  << (normal ? 1 : 0)
            << std::setw(6)  << level
            << std::setw(6)  << address
            << std::setw(6)  << linkIndex;
        return ss.str();
    }
};

struct ArrayTableEntry{
    explicit ArrayTableEntry(int arrayIndex, DataType indexType, DataType elementType, int compositeTypeReference,
                            int low, int high, int elementSize, int size);
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

    // to string

    static::std::string getHeader() {
        std::stringstream ss;
        ss << std::left 
            << std::setw(5) << "idx"
            << std::setw(8) << "inxtyp"
            << std::setw(8) << "eltyp"
            << std::setw(6) << "ref"
            << std::setw(6) << "low"
            << std::setw(6) << "high"
            << std::setw(6) << "elsz"
            << std::setw(6) << "size";
        return ss.str();
    }

    std::string toString() const {
        std::stringstream ss;
        ss << std::left 
            << std::setw(5) << arrayIndex
            << std::setw(8) << static_cast<int>(indexType)
            << std::setw(8) << static_cast<int>(elementType)
            << std::setw(6) << compositeTypeReference
            << std::setw(6) << low
            << std::setw(6) << high
            << std::setw(6) << elementSize
            << std::setw(6) << size;
        return ss.str();
    }
};

struct BlockTableEntry {
    explicit BlockTableEntry(int blockIndex, int last, int latestParameter, int parameterSize, int variableSize);
    // Indeks entri block (setiap block mewakili
    // prosedur, fungsi, atau record type definition).
    int blockIndex;

    // Pointer/indeks ke identifier terakhir yang dideklarasikan
    // di dalam block tersebut (menghubungkan field record,
    // parameter, atau variabel lokal).
    int last;

    // Pointer/indeks ke parameter terakhir dari prosedur/fungsi pada block tersebut. Jika block adalah record, nilainya 0.
    int latestParameter;

    // Total ukuran parameter block  (dalam byte/unit memori).
    int parameterSize;

    // Total ukuran variabel lokal block (dalam byte/unit memori)
    int variableSize;

    static std::string getHeader() {
        std::stringstream ss;
        ss << std::left 
            << std::setw(5)  << "idx"
            << std::setw(8)  << "last"
            << std::setw(10) << "lparam"
            << std::setw(8)  << "psize"
            << std::setw(8)  << "vsize";
        return ss.str();
    }

    // 2. Fungsi untuk mencetak baris data Block
    std::string toString() const {
        std::stringstream ss;
        ss << std::left 
            << std::setw(5)  << blockIndex
            << std::setw(8)  << last
            << std::setw(10) << latestParameter
            << std::setw(8)  << parameterSize
            << std::setw(8)  << variableSize;
        return ss.str();
    }
};