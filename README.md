# Task 1 — Building a Balatro-Like Run
**Mata Kuliah:** Desain Pattern  
**Topik:** Core Loop & System Thinking (Invariant vs Mutable Architecture)  

---

## 📌 Deskripsi Program
Program ini merupakan simulasi *game loop* berbasis teks bergaya Balatro yang dibangun menggunakan C++ dengan fokus utama pada **kedisiplinan arsitektur perangkat lunak (*architectural discipline*)**. Program memisahkan secara tegas antara struktur alur permainan yang kaku (*invariant*) dan komponen logika permainan yang dapat berubah (*mutable*) dengan menerapkan prinsip **Object-Oriented Programming (OOP)**, **Interface Abstraction**, dan **Dependency Injection**.

Program mengeksekusi simulasi run sebanyak **tepat 3 round** dengan 6 urutan fase mutlak pada setiap round:
1. `Generate input`: Menghasilkan data kartu/aksi (`TurnInput`).
2. `Compute base score`: Menghitung skor dasar dari input.
3. `Compute reward`: Menghitung konversi uang dari nilai skor dasar.
4. `Update money`: Menambahkan nilai reward ke saldo uang pemain.
5. `Shop phase`: Menampilkan penawaran item di toko.
6. `Advance round`: Melanjutkan status permainan ke round berikutnya.

---

## 🏗️ Implementasi Modifikasi Wajib
Arsitektur sistem ini membuktikan keandalannya melalui dua modifikasi perilaku tanpa mengubah satu baris kode pun di dalam controller utama (`RunSession`):
1. **Modification 1 — Replace Input Generator**:
   Mengganti generator dasar dengan `RandomInputGenerator` yang mengimplementasikan `IInputGenerator` untuk menghasilkan nama kombinasi kartu poker dan skor secara dinamis/acak.
2. **Modification 2 — Change Reward Logic**:
   Mengganti aturan reward dengan `BonusRewardRule` yang mengimplementasikan `IRewardRule`, di mana nilai perolehan uang tidak lagi bernilai sama persis dengan skor dasar (menerapkan bonus flat `+2` dan pengganda `2x` khusus pada round genap).

---

## 📝 Reflection (Mandatory — 200–300 words)

> **Catatan:** Refleksi di bawah ini berisi **223 kata** dan menjawab 4 pertanyaan kunci yang disyaratkan:

Struktur invariant dalam program ini adalah alur eksekusi inti (core loop) yang diatur oleh kelas RunSession. Urutan 6 fase pada setiap round bersifat mutlak dan tidak boleh diubah: (1) Generate input, (2) Compute base score, (3) Compute reward, (4) Update money, (5) Shop phase, dan (6) Advance round. Jumlah total 3 round serta aturan bahwa pembaruan uang hanya terjadi pada fase reward juga merupakan bagian dari invariant sistem.

Bagian mutable mencakup seluruh algoritma dan perilaku konkret yang didelegasikan ke antarmuka (interface): cara masukan dihasilkan (IInputGenerator), perhitungan skor (IScoringRule), konversi skor menjadi uang (IRewardRule), serta penyajian penawaran barang pada toko (ShopSystem). Implementasi konkret seperti FixedInputGenerator, RandomInputGenerator, DefaultRewardRule, maupun BonusRewardRule dapat diganti secara dinamis tanpa mengganggu controller.

RunSession menerapkan prinsip Dependency Inversion dan Polymorphism. Kelas tersebut hanya bergantung pada abstraksi murni IInputGenerator, bukan pada kelas konkret. Ketika FixedInputGenerator diganti menjadi RandomInputGenerator, kontrak method generateInput() tetap identik. RunSession menerima objek melalui constructor injection, sehingga perubahan perilaku masukan terjadi di luar kontroler tanpa memerlukan modifikasi kode internal sedikit pun (Open/Closed Principle).

Jika perhitungan skor dimasukkan langsung ke dalam RunSession, terjadi pelanggaran prinsip Single Responsibility Principle (SRP) dan tercipta kopling ketat (tight coupling). Controller kehilangan sifat invariant karena setiap variasi aturan skor (misalnya efek kartu baru atau multiplier) akan memaksa modifikasi pada RunSession. Hal ini meningkatkan risiko regresi, merusak integritas alur utama, dan mempersulit pengujian unit (unit testing).

---

## ⚙️ Petunjuk Kompilasi dan Eksekusi

Program dapat dikompilasi menggunakan compiler C++ standar (C++17 ke atas seperti `g++` / MinGW):

```bash
# Kompilasi program
g++ -std=c++17 -Wall -Wextra -O2 task1BalatroRun.cpp -o task1BalatroRun.exe

# Menjalankan program di terminal
./task1BalatroRun.exe
