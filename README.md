# Arion Interpreter 🦫

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

Program ini adalah sebuah compiler dan interpreter lengkap untuk bahasa pemrograman **Arion**, yang dikembangkan sebagai tugas besar IF2224 (Teori Bahasa Formal dan Automata). Arsitektur program ini mengadopsi konsep yang mirip dengan **Java Virtual Machine (JVM)**, di mana kode sumber terlebih dahulu dikompilasi menjadi representasi tingkat menengah (Intermediate Code) sebelum akhirnya dieksekusi oleh sebuah *interpreter* (Virtual Machine).

Program ini mencakup seluruh tahapan kompilasi hingga eksekusi program:
1. **Lexical Analysis (Lexer)**: Mengubah source code menjadi urutan token.
2. **Syntax Analysis (Parser)**: Membangun *Concrete Syntax Tree* (CST) berdasarkan aturan tata bahasa Arion.
3. **Semantic Analysis**: Membangun *Decorated Abstract Syntax Tree* (AST), melakukan *type checking*, manajemen *symbol table*, dan validasi lingkup (*scope*).
4. **Intermediate Code Generation (ICG)**: Mentransformasi AST menjadi *Intermediate Representation* berupa **P-Code linear** menggunakan teknik *backpatching* untuk meratakan (*control flow*).
5. **Virtual Machine (Execution Engine)**: Mengeksekusi instruksi P-Code secara berurutan menggunakan *Runtime Stack*, penanganan *activation record*, dan fungsi operasi logika.

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

- `l`  : jalankan *lexer* saja — test case di (`../test/milestone-1/valid_test/`)
- `p`  : jalankan *lexer* + *parser* — test case di (`../test/milestone-2/valid_test/`)
- `s`  : jalankan *lexer* + *parser* + *semantic analyzer* — test case di (`../test/milestone-3/valid_test/`)
- `i`  : jalankan *seluruh tahapan* (hingga *Intermediate Code* & *Interpreter*) — test case di (`../test/milestone-4/valid_test/`)

Contoh:
- Menjalankan Lexer
```bash
./main l valid1.txt
```

- Menjalankan Full Compiler & Interpreter
```bash
./main i valid8_recursive_factorial.txt
```


### 3. Output

- **Mode Lexer (`l`)**:
  Hasil tokenisasi disimpan di `../test/milestone-1/output` dengan nama file `<nama_file>-result.txt`.

- **Mode Parser (`p`)**:
  Hasil *parse tree* disimpan di `../test/milestone-2/output` dengan nama file `<nama_file>-parse-tree.txt`. Parse tree juga ditampilkan di terminal.

- **Mode Semantic Analyzer (`s`)**:
  Hasil *Decorated AST* dan *Symbol Table* disimpan di `../test/milestone-3/output` dengan nama file `<nama_file>-ast.txt`. Error dan warning akan muncul di terminal.

- **Mode Interpreter (`i`)**:
  - **Intermediate Code**: Daftar instruksi P-Code linear hasil kompilasi disimpan di `../test/milestone-4/output` dengan nama file `<nama_file>-intermediate-code.txt`.
  - **Execution Output**: Hasil eksekusi disimpan di `../test/milestone-4/output` dengan nama file `<nama_file>-output.txt`.

### 4. Membersihkan Compiled Files

Untuk menghapus file object dan executable, jalankan:

```bash
cd src
make clean
```
## Pembagian Tugas Kontribusi Milestone 1
| No | NIM | Kontribusi |
|----|-------|-----------|
| 1  | 13524008 | 20% |
| 2  | 13524030 | 20% |
| 3  | 13524074 | 20% |
| 4  | 13524096 | 20% |
| 5  | 13524110 | 20% |

## Pembagian Tugas Kontribusi Milestone 2
| No | NIM | Kontribusi |
|----|-------|-----------|
| 1  | 13524008 | 20% |
| 2  | 13524030 | 20% |
| 3  | 13524074 | 20% |
| 4  | 13524096 | 20% |
| 5  | 13524110 | 20% |

## Pembagian Tugas Kontribusi Milestone 3
| No | NIM | Kontribusi |
|----|-------|-----------|
| 1  | 13524008 | 20% |
| 2  | 13524030 | 20% |
| 3  | 13524074 | 20% |
| 4  | 13524096 | 20% |
| 5  | 13524110 | 20% |

## Pembagian Tugas Kontribusi Milestone 4 & 5
| No | NIM | Kontribusi |
|----|-------|-----------|
| 1  | 13524008 | 20% |
| 2  | 13524030 | 20% |
| 3  | 13524074 | 20% |
| 4  | 13524096 | 20% |
| 5  | 13524110 | 20% |
