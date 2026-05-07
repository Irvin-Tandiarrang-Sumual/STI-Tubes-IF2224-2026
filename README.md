# Arion Compiler - Part Parser 🦫

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

Program yang dibuat saat ini merupakan bagian dari compiler yang akan menjadi tujuan akhir dari tugas besar IF2224 (TBFO) bukan Stima, hehe, saat ini. Saat ini program baru sampai pada tahap parser. Di mana parser bertujuan untuk memastikan urutan token membentuk struktur sintaks yang valid, mendeteksi dan melaporkan kesalahan sintaks (memanfaatkan Panic Mode Recovery), dan mempersiapkan representasi program untuk tahap berikutnya. Parser akan menghasilkan parse tree yang akan dipakai di tahap berikutnya! yippie :D

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

Setelah kompilasi berhasil, jalankan program dengan:

```bash
./main <mode> <nama_file>
Mode:
 l -> menjalankan lexer
 p -> menjalankan lexer & parser

Nama File harus yang berada di `../test/milestone-2/valid_test/`
```

### 3. Output

Hasil parsing (parse tree) dalam bentuk file `.txt` akan diberikan di `../test/milestone-2/output`, di mana nama file output merupakan `nama file input` + `-parse-tree.txt`dan parse tree yang sama akan di-print ke terminal beserta error message yang ada.

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
