#pragma once

#include <string>
#include <variant>

enum class OpCode {
    LIT, // Load Literal Memasukkan nilai literal v ke dalam stack
    LOD, // Load Value Memuat nilai dari address a
    STO, // Store Value Menyimpan nilai ke address a
    LDA, // Load address Memuat alamat a ke dalam stack 
    LDI, // Load Indirect Memuat nilai dari alamat yang ada di stack teratas
    STI, // Store Indirect Menyimpan nilai ke alamat yang ada di stack teratas
    CHK, // Change Level Mengubah level eksekusi
    CAL, // Call Memanggil fungsi di garis l
    INT, // Initiate Memory Membuat memory dengan ukuran m
    JMP, // Unconditional Jump Lompat ke garis l tanpa kondisi apapun
    JPC, // Conditional Jump Lompat ke garis l bila kondisi tidak memenuhi
    OPR, // Operation Memanggil operasi o (Termasuk fungsi bawaan Arion)
    RET // Return Keluar dari fungsi atau prosedur
};

// Instruksi OPR (Jalankan seperti ini OPR [value_hierarchy] [operation_no]
enum class OprCode {
    NEG   = 1, // Negasi value dari stack teratas

    ADD   = 2, // Menambah value stack teratas dengan value stack teratas satunya lagi

    SUB   = 3, // Mengurangi value stack teratas dengan value stack teratas satunya lagi

    MUL   = 4, // Mengali value stack teratas dengan value stack teratas satunya lagi

    DIV   = 5, // Membagi value stack teratas dengan value stack teratas satunya lagi yang menghasilkan integer

    MOD   = 6, // Modulus value stack teratas dengan value stack teratas satunya lagi

    EQL   = 7, // Membandingkan kedua value teratas dari stack untuk melihat
                // apakah kedua value tersebut sama (Conditionals)
    NEQ   = 8, // Membandingkan kedua value teratas dari stack untuk melihat apakah
                // kedua value tersebut tidak sama (Conditionals)
    LSS   = 9, // Membandingkan kedua value teratas dari stack untuk melihat
                // apakah value pertama kurang dari value kedua (Conditionals)

    GEQ   = 10, // Membandingkan kedua value teratas dari stack untuk melihat apakah value
                // pertama lebih dari sama dengan value kedua (Conditionals)

    GTR   = 11, // Membandingkan kedua value teratas dari stack untuk melihat apakah valu
                // pertama lebih dari value kedua (Conditionals)

    LEQ   = 12, // Membandingkan kedua value teratas dari stack untuk melihat apakah value
                // pertama kurang dari sama dengan value kedua (Conditionals)

    WRT   = 13, // Menulis output yang sudah dimuat

    WRTLN = 14, // Menulis output yang sudah dimuat lalu diberikan newline

    RDIV = 15 // Membagi value stack teratas dengan value stack teratas satunya lagi yang menghasilkan bilangan real
};

class Instruction {
    private:
        OpCode op_;
        int level_;
        std::variant<int, double, char, std::string> operand_;

    public:
        Instruction(OpCode op, int level, std::variant<int, double, char, std::string> operand);

        OpCode getOp() const;
        int getLevel() const;
        std::variant<int, double, char, std::string> getOperand() const;

        void setOp(OpCode op);
        void setLevel(int level);
        void setOperand(std::variant<int, double, char, std::string> operand);

        std::string toString(int lineNumber) const;
};

std::string opCodeToString(OpCode op);
std::string oprCodeToString(OprCode op);