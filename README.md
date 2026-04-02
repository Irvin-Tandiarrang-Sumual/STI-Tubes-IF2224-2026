# Arion Compiler - Part Lexer

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

Program yang dibuat saat ini merupakan bagian dari compiler yang akan menjadi tujuan akhir dari tugas besar IF2224 (TBFO) bukan Stima, hehe, saat ini. Saat ini program baru sampai pada tahap lexer. Di mana lexer berfungsi untuk melakukan lexical analysis untuk mengubah input yang berisikan source code bahasa arion ke dalam token-token yang akan dimanfaatkan di tahap berikutnya :)

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
./main
```

Program akan menampilkan prompt untuk memasukkan nama file input:


### 3. Memberikan Input

Masukkan path atau nama file yang ingin dianalisis. Program hanya dapat membuka file `.txt` yang berada di `../test/milestone-1/valid_test/`

Contoh:
```
Masukkan path/nama file input: valid1.txt
```

### 4. Output

Hasil tokenisasi dalam bentuk file `.txt` akan diberikan di `../test/milestone-1/output` dan error yang muncul akan diberikan di terminal.

### 5. Membersihkan Compiled Files

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
