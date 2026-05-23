# Arion Compiler - Part Semantic Analyzer 🦫

## Strategi Algoritma
merupakan nama kelompok kami yang jika disingkat menjadi STI
| No | Nama | NIM |
|----|-------|-----------|
| 1  | Agatha Tatianingseto | 13524008 |
| 2  | Irvin Tandiarrang Sumual | 13524030 |
| 3  | Bernhard Aprillio Pramana | 13524074 |
| 4  | Moreno Syawali Ganda Sugita | 13524096 |
| 5  | Jennifer Khang | 13524110 |
## Deskripsi Program

Program yang dibuat saat ini merupakan bagian dari compiler yang akan menjadi tujuan akhir dari tugas besar IF2224 (TBFO) bukan Stima, hehe, saat ini. Saat ini program baru sampai pada tahap semantic analysis. Di mana Semantic Analysis bertujuan untuk memastikan bahwa program input memenuhi makna semantic dari bahasa Arion. Pengecekan yang dilakukan terdiri dari:
1. Type Checking (Pengecekan Tipe)
Memastikan operator dan operand memiliki tipe yang kompatibel.
2. Symbol Table Management (Pengecekan Deklarasi)
Memastikan setiap identifier (variabel/fungsi) telah dideklarasikan sebelum. digunakan dan tidak dideklarasikan ulang dalam cakupan yang sama.
3. Scope Resolution (Pengecekan Lingkup)
Menentukan validitas akses variabel berdasarkan hierarki blok kode.
4. Control Flow Validation (Pengecekan Alur Kontrol)
Memastikan struktur alur kontrol yang logis.



## Requirements

Untuk menjalankan program ini, diperlukan:
- **C++ 17**
- **G++**
- **Make** (GNU Make)

## Cara Instalasi dan Penggunaan Program

### 1. Kompilasi Program

Masuk ke direktori `src` dan jalankan perintah:

```bash
cd src
make clean
make
```

Perintah ini akan mengkompilasi semua file source code dan menghasilkan executable `main`.

### 2. Menjalankan Program

Setelah kompilasi berhasil, jalankan program dari direktori `src` dengan sintaks:

```bash
./main <mode> <nama_file>
```

Mode yang tersedia:

- `l`  : jalankan *lexer* saja — gunakan test case dari (`../test/milestone-1/valid_test/`)
- `p`  : jalankan *lexer* + *parser* — gunakan test case dari (`../test/milestone-2/valid_test/`)
- `s`  : jalankan *lexer* + *parser* + *semantic analyzer* — gunakan test case dari (`../test/milestone-3/valid_test/`).

Contoh:
- Menjalankan Lexer
```bash
./main l valid1.txt
```

- Menjalankan Parser
```bash
./main p ifelse.txt
```

- Menjalankan Semantic Analyzer
```bash
./main s Jumbo.txt
```


### 3. Output
- Setelah Menjalankan Lexer
Hasil tokenisasi dalam bentuk file `.txt` akan diberikan di `../test/milestone-1/output` dan error yang muncul akan diberikan di terminal.

- Setelah Menjalankan Parser
Hasil parsing (parse tree) dalam bentuk file `.txt` akan diberikan di `../test/milestone-2/output`, di mana nama file output merupakan `nama file input` + `-parse-tree.txt`dan parse tree yang sama akan di-print ke terminal beserta error message yang ada.

- Setelah Menjalankan Semantic Analyzer
Hasil Decorated AST dan Symbol Table dalam bentuk file `.txt` akan diberikan di `../test/milestone-3/output` dan error yang muncul akan diberikan di terminal beserta warning jika ada.


### 4. Membersihkan Compiled Files

Untuk menghapus file object dan executable, jalankan:

```bash
cd src
make clean
```

## Pembagian Tugas Kontribusi
| No | NIM | Kontribusi |
|----|-------|-----------|
| 1  | 13524008 | (67/335) * 100 |
| 2  | 13524030 | (67/335) * 100 |
| 3  | 13524074 | (67/335) * 100 |
| 4  | 13524096 | (67/335) * 100 |
| 5  | 13524110 | (67/335) * 100 |
