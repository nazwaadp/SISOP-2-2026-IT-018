# Laporan Resmi Praktikum Sistem Operasi Modul 2

## Identitas
- Nama: Nazwa Aulia Dwi Purnomo
- NRP: 5027251018
- Kelas: B
- Kode Asisten: KENZ

## Struktur Repository
```bash
SISOP-2-2026-IT-018
├── README.md
├── assets
│ ├── soal_1 
│ ├── soal_2 
│ └── soal_3 
├── soal_1
│ ├── brankas_kedai
│ │ ├── buku_hutang.csv
│ │ └── daftar_penunggak.txt
│ ├── buku_hutang.csv
│ ├── kasir_muthu.c
│ └── rahasia_muthu.zip
├── soal_2
│ ├── contract.txt
│ ├── contract_daemon.c
│ └── work.log
└── soal_3
```

## Soal 1 - KASBON WARGA KAMPUNG DURIAN RUNTUH
### Penjelasan Soal 
Soal ini meminta untuk membuat program C bernama `kasir_muthu.c` untuk membantu Uncle Muthu mengamankan data penting yang ada di file [`buku_hutang.csv`](soal_1/buku_hutang.csv).Pada soal diceritakan bahwa komputer kasir terkena virus, jadi  data-data di kasir harus segera diamankan sebelum terjadi hal yang tidak diinginkan.

Program harus dijalankan secara urut menggunakan konsep *Sequential Process*, yaitu parent process nanti membuat child process satu per satu untuk menjalankan tugas tertentu, lalu parent harus menunggu child tersebut selesai sebelum lanjut ke tugas berikutnya. Karena itu, soal ini secara khusus meminta penggunaan kombinasi:

1. `fork()`
2. `exec()`
3. `waitpid()`

Pada soal ini juga dilarang keras menggunakan `system()`, jadi semua command harus dijalankan melalui `exec()`.

Proses dalam program yang diminta dalam soal sebagai berikut: <br>
1. Membuat folder [`brankas_kedai`](soal_1/brankas_kedai)
2. Menyalin file [`buku_hutang.csv`](soal_1/buku_hutang.csv) ke folder tersebut
3. Mencari semua data pelanggan dengan status **"Belum Lunas"**
4. Menyimpan hasil pencarian itu ke file [`daftar_penunggak.txt`](soal_1/brankas_kedai/daftar_penunggak.txt)
5. Mengompres folder [`brankas_kedai`](soal_1/brankas_kedai) menjadi [`rahasia_muthu.zip`](soal_1/rahasia_muthu.zip)
6. Jika ada satu proses yang gagal, program harus langsung berhenti
7. Jika semua berhasil, program menampilkan pesan sukses.
---

### Penyelesaian Soal
Program dibuat menggunakan beberapa library sebagai berikut:

- `stdio.h` untuk menampilkan output ke terminal
- `stdlib.h` untuk fungsi `exit()`
- `unistd.h` untuk `fork()` dan `execlp()`
- `sys/types.h` untuk tipe data `pid_t`
- `sys/wait.h` untuk `waitpid()` dan pengecekan status child process

Di dalam `main()`, ada dua variabel penting:
- `pid_t pid;` untuk menyimpan ID proses hasil `fork()`
- `int status;` untuk menyimpan status hasil eksekusi child process

Dimulai dari kode pada program yang digunakan untuk menghapus hasil lama jika masih ada. Hal ini dilakukan supaya saat program dijalankan ulang, folder dan file hasil percobaan sebelumnya tidak bentrok dengan hasil baru. <br>
Command yang dijalankan di awal adalah:

```bash
rm -rf brankas_kedai rahasia_muthu.zip
```
Langkah ini dilakukan dengan tujuan mempermudah juga program lebih rapi saat diuji berkali-kali.

#### 1. Program menggunakan *sequential process* dengan parent dan child process.

Pada soal poin 1, program ini dibuat dengan konsep *sequential process*. Artinya, semua proses berjalan berurutan, bukan bersamaan. <br>
Cara kerjanya adalah: <br>
1. parent process membuat child process dengan `fork()`
2. child process menjalankan command tertentu dengan `execlp()`
3. parent process menunggu child selesai menggunakan `waitpid()`
4. parent memeriksa apakah proses child berhasil atau gagal
5. kalau berhasil, parent lanjut ke langkah berikutnya
6. kalau gagal, program langsung berhenti

Bagian ini berulang di setiap langkah. 
```bash
pid = fork();
if (pid < 0) {
    printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(1);
}
```
`fork()`  dipakai untuk membuat proses baru
kalau hasil `fork()` kurang dari 0, maka proses child gagal dibuat. Jika gagal, program langsung menampilkan pesan error dan berhenti. Setelah itu, jika `pid == 0`, berarti bagian kode tersebut dijalankan oleh child process, sedangkan parent akan menunggu hasilnya. <br>
Fungsi `waitpid(pid, &status, 0);` sangat penting karena parent tidak boleh lanjut sebelum child selesai. Ini yang membuat proses tetap berjalan urut, sesuai konsep sequential process.

#### 2. Child process pertama membuat folder brankas_kedai

Di dalam program, terdapat kode dimana parent membuat child process baru yakni `pid = fork();`.
Lalu jika child process berhasil dibuat dan `pid == 0`, child menjalankan: <br>
``` bash
execlp("mkdir", "mkdir", "brankas_kedai", NULL);
```
Tujuannya adalah membuat folder khusus yang akan dipakai sebagai tempat penyimpanan file penting. Jika `execlp()` gagal menjalankan command `mkdir`, maka child akan menampilkan pesan: <br>

``` bash
printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
exit(1);
```

Setelah child selesai, parent menunggu dengan `waitpid(pid, &status, 0);`, Lalu parent cek apakah proses benar-benar berhasil menggunakan kode berikut:

```bash
if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(1);
}
```

Jadi, folder [`brankas_kedai`](soal_1/brankas_kedai) harus berhasil dibuat dulu. Jika gagal, program tidak boleh lanjut ke proses berikutnya.

#### 3. Child process kedua menyalin file buku_hutang.csv ke brankas_kedai

Setelah folder berhasil dibuat, parent membuat child process kedua untuk menyalin file [`buku_hutang.csv`](soal_1/buku_hutang.csv) ke dalam folder [`brankas_kedai.`](soal_1/brankas_kedai) menggunakan cp.<br>
Pada kode, child menjalankan:

``` bash
execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
```

Tujuan langkah ini adalah mengamankan file asli dengan membuat salinannya di dalam folder brankas. Langkah ini harus dilakukan setelah `mkdir` karena file tujuan akan disalin ke folder [`brankas_kedai`](soal_1/brankas_kedai), jadi foldernya harus sudah ada terlebih dahulu.

Setelah child selesai menjalankan cp, parent kembali menunggu: <br>
`waitpid(pid, &status, 0);` <br>
Kemudian statusnya diperiksa, jika file [`buku_hutang.csv`](soal_1/brankas_kedai/buku_hutang.csv) tidak ada, atau penyalinan gagal, maka status proses tidak akan sukses dan program langsung berhenti.

#### 4. Child process ketiga mencari data dengan status "Belum Lunas" lalu menyimpannya ke daftar_penunggak.txt

Setelah file CSV berhasil masuk ke dalam folder [`brankas_kedai/`](soal_1/brankas_kedai), parent membuat child process ketiga untuk mencari seluruh data pelanggan yang masih memiliki status utang **"Belum Lunas"**.

Pada kode, child menjalankan:

``` bash
execlp("sh", "sh", "-c",
       "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
       NULL);
```
Artinya, grep **'Belum Lunas'** dipakai untuk mencari semua baris yang mengandung teks **Belum Lunas** dan file yang dibaca adalah `brankas_kedai/buku_hutang.csv`
hasil pencarian disimpan ke file `brankas_kedai/daftar_penunggak.txt`

Di sini digunakan `sh -c` karena command mengandung tanda `>`, yaitu *redirect output ke file*. Redirect seperti ini tidak bisa dipakai langsung kalau hanya memanggil grep biasa lewat `execlp()`. Karena itu, command dibungkus lewat shell. <br>
Hal ini penting karena soal tidak hanya meminta file CSV diamankan, tetapi juga meminta data pelanggan yang belum melunasi hutang dipisahkan ke file tersendiri.

#### 5. Child process keempat mengompres folder menjadi rahasia_muthu.zip

Setelah folder [`brankas_kedai/`](soal_1/brankas_kedai) berisi file [`buku_hutang.csv`](soal_1/brankas_kedai/buku_hutang.csv) dan [`daftar_penunggak.txt`](soal_1/brankas_kedai/daftar_penunggak.txt), parent membuat child process keempat untuk mengompres folder tersebut menjadi file ZIP. child menjalankan:<br>

``` bash
execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
```

Artinya, zzip digunakan untuk membuat file kompresi dan `-r` berarti folder dikompres secara **rekursif** termasuk semua isi di dalamnya. [`rahasia_muthu.zip`](soal_1/rahasia_muthu.zip) adalah nama file output
[`brankas_kedai`](soal_1/brankas_kedai) adalah folder yang akan dikompres

#### 6. Setiap proses dicek keberhasilannya, dan program berhenti jika ada yang gagal

Hal penting dalam soal ini adalah program harus langsung berhenti jika ada langkah yang gagal. Pada kode, hal ini diterapkan pada setiap proses setelah `waitpid().` <br>
Bagian yang dipakai untuk pengecekan adalah:

```bash
if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(1);
}
```

`WIFEXITED(status)` mengecek apakah child process selesai dengan normal, sedangkan `WEXITSTATUS(status)` mengambil nilai exit status dari child. Jika exit status tidak sama dengan 0, berarti proses dianggap gagal.

#### 7. Jika semua langkah berhasil, program menampilkan pesan

Ketika semua proses dari awal sampai akhir berhasil, maka di bagian terakhir program akan menampilkan pesan:

```bash
printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");
```
Jadi, jika pesan diatas muncul, berarti seluruh alur pengamanan data sudah selesai tanpa kendala dan menandakan bahwa: <br>
- Folder [`brankas_kedai/`](soal_1/brankas_kedai) berhasil dibuat
- File [`buku_hutang.csv`](soal_1/brankas_kedai/buku_hutang.csv) berhasil disalin
- Data pelanggan yang "Belum Lunas" berhasil dipisahkan
- File [`daftar_penunggak.txt`](soal_1/brankas_kedai/daftar_penunggak.txt) berhasil dibuat
f- Folder berhasil dikompres menjadi [`rahasia_muthu.zip`](soal_1/rahasia_muthu.zip)

---

### Dokumentasi
#### 1. Compile program kasir_muthu.c untuk menjalankan program dan output berhasil

![Compile program kasir_muthu.c untuk menjalankan program dan output berhasil](<assets/soal_1/0-1-compile dan tampilan sukses.png>)

#### 2. Struktur file setelah program berhasil

![Struktur file setelah program berhasil](<assets/soal_1/0-2-cek tree setelah berkas diamankan.png>)

#### 3. Menguji kondisi gagal dengan mengubah nama file buku_hutang.csv

![Menguji kondisi gagal dengan mengubah nama file buku_hutang.csv](<assets/soal_1/0-3-cek jika file hilang.png>)

#### 4. Ouput error karena file sumber hilang/file tidak ditemukan

![Ouput error karena file sumber hilang/file tidak ditemukan.](<assets/soal_1/0-4-output untuk error file hilang.png>)

---
### Kendala
Tidak ada.

---

## Soal 2 - THE WORLD NEVER STOPS, EVEN WHEN YOU FEEL TIRED.

### Penjelasan Soal
Pada soal, diminta untuk membuat sebuah program C bernama `contract_daemon.c` yang berjalan sebagai **daemon**. <br>
Di soal ini, daemon yang dibuat memiliki tugas untuk menjaga 2 hal, yakni terus menulis aktivitas kerja ke file `work.log` dan menjaga agar file `contract.txt` selalu ada serta selalu berada di kondisi yang benar.

Beberaapa poin yang diminta dalam soal sebagai berikut: <br>
1. Membuat file `contract_daemon.c` yang isinya menjadi pusat seluruh kode program.
2. Daemon harus menulis log ke `work.log` yang setiap 5 detik sekali, daemon harus menambahkan satu baris log ke file `work.log`. Isi lognya harus berbentuk `still working… [status]` dengan status acak dari tiga status; `[awake]`, `[drifting]`, `[numbness]`
3. Saat program mulai berjalan, harus membuat `contract.txt` yang file tersebut harus memiliki dua baris ; kutipan tetap dan waktu pembuatan file dengan format `created at: <timestamp>`
4. Jika `contract.txt` dihapus, daemon tidak boleh diam. File itu harus dibuat ulang dalam waktu sekitar 1–2 detik. Saat file dibuat ulang, isi baris pertamanya tetap sama, tetapi baris keduanya berubah menjadi `restored at: <timestamp>`
5. Jika isi `contract.txt` diubah, daemon harus menganggap kontrak dilanggar dan menulis `contract violated.` ke `work.log`
serta mengembalikan isi `contract.txt` ke bentuk semula
6. Jika daemon dihentikan, tulis pesan terakhir ke `work.log` berupa `We really weren’t meant to be together` ke dalam `work.log`.
7. Semua operasi harus dilakukan lewat program C, bukan dibuat manual.
---

### Penyelesaian Soal

Program dibuat dengan konsep daemon yang terus berjalan di balik layar. Supaya program bisa melakukan itu, dibutuhkan beberapa library standar C dan sistem operasi Linux, antara lain:
- `stdio.h` dipakai untuk `FILE`, `fopen`, `fprintf`, `fgets`, dan `fclose`
- `stdlib.h` dipakai untuk `exit`, `rand`, dan `srand`
- `unistd.h` dipakai untuk `fork`, `setsid`, `chdir`, `sleep`, dan `close`
- `sys/stat.h` dipakai untuk `stat` dan `umask`
- `time.h` dipakai untuk pengambilan timestamp
- `string.h` dipakai untuk `strcmp` dan `strncmp`
- `signal.h` dipakai untuk menangani sinyal saat daemon dihentikan. Semua ini terlihat langsung di source code yang kamu kirim. 

Dimulai dari kode dibawah ini:
```c
int running = 1;
const char *original_quote = "“A promise to keep going, even when unseen.”\n";
```

`running` adalah penanda apakah loop daemon masih terus jalan atau harus berhenti.
Sedangkan `original_quote` adalah kutipan tetap yang ada di baris pertama [`contract.txt`](soal_2/contract.txt)

#### 1. Program dibuat dalam file contract_daemon.c

Diawali membuat file bernama `contract_daemon.c` yang isinya logika daemon. Di dalam file ini, program dibagi menjadi beberapa bagian agar tidak jadi satu dan ditumpuk di dalam `main()`.
- variabel global
- fungsi handler sinyal
- fungsi pembuat timestamp
- fungsi penulis log rutin
- fungsi penulis log pelanggaran
- fungsi penulis log perpisahan
- fungsi pembuat file contract
- fungsi pemulih file contract
- fungsi pengecek apakah isi file berubah
- fungsi `main()` yang menjadi pusat daemonisasi dan loop.<br> 

#### 2. Daemon menulis `still working...` ke work.log
Pada program, tugas ini dikerjakan oleh fungsi `write_log_message()`. Di dalam fungsi itu, langkah-langkahnya adalah:
1. Mengambil waktu sekarang
2. Memilih angka acak dari `0` sampai `2`
3. Mengubah angka itu menjadi salah satu status
4. Membuka file `work.log` dengan `mode append`
5. Menulis satu baris log
6. Menutup file lagi. Semua langkah ini terlihat di fungsi `write_log_message()`. 

``` bash
random_status = rand() % 3;
```

Artinya, angka acak yang mungkin muncul hanya: `0`, `1`, `2` dimana kondisi tersbut sama dengan; `0` > `[awake]`, `1` > `[drifting]`, `2` > `[numbness]` dan setelah status dipilih, file `work.log` dibuka dengan:

``` bash
log_file = fopen("work.log", "a");
```

Mode `"a"` berarti `append`, artinya isi lama tidak dihapus. Log baru selalu ditambahkan di bagian bawah file. Setelah itu ditulis sesuai format pada soal, yakni:

```c
fprintf(log_file, "[%s] still working... %s\n", timestamp, status_text);
```

Agar hasil acaknya tidak selalu sama, di dalam `main()` ada `srand(time(NULL));` yang berfungsi sebagai *seed random* berdasarkan waktu saat program mulai. 

Kemudian ada fungsi `write_log_message()` ini dipanggil terus di dalam loop utama:

```c
while (running) {
    write_log_message();
    ...
    sleep(5); // artinya jeda 5 detik
}
```

#### 3. Saat program mulai berjalan, daemon membuat contract.txt

File [`contract.txt`](soal_2/contract.txt) dibuat otomatis saat daemon mulai dan dikerjakan oleh fungsi `create_contract_file()`.
1. Mengambil timestamp sekarang
2. Membuka [`contract.txt`](soal_2/contract.txt) dengan mode `"w"`
3. Menulis kutipan tetap ke baris pertama
4. Menulis `created at: <timestamp>` ke baris kedua
5. Menutup file. Implementasi ini terlihat jelas di fungsi `create_contract_file()`. 

```bash
create_contract_file();
```

Pemanggilan fungsi ini dilakukan di `main()` setelah proses daemonisasi selesai. Begitu daemon sudah resmi aktif di background, file [`contract.txt`](soal_2/contract.txt) langsung dibuat. 

#### 4. Jika contract.txt dihapus, file dibuat kembali dalam 1–2 detik

Pemulihkan [`contract.txt`](soal_2/contract.txt) jika file itu hilang. Di kode, cek keberadaan file dilakukan dengan:

```c
if (stat("contract.txt", &file_info) != 0)
```

Fungsi `stat()` dipakai untuk mengecek apakah file ada. Kalau return-nya bukan `0`, berarti file tidak ditemukan atau tidak bisa diakses. Di dalam loop utama, kalau kondisi ini terpenuhi, maka daemon melakukan:

```c
sleep(1);
restore_contract_file();
``` 

Fungsi `restore_contract_file()` isinya mirip dengan fungsi pembuatan awal, tetapi ada perbedaan penting pada baris kedua. Saat file dipulihkan, program menulis:

```text
restored at: <timestamp>
```

bukan `created at:` lagi. Jadi daemon bisa membedakan antara file yang baru dibuat pertama kali dan file yang dibuat ulang setelah hilang. Implementasi ini terlihat langsung di fungsi `restore_contract_file()`. 

#### 5. Jika isi contract.txt diubah, daemon menulis `contract violated.` lalu mengembalikannya

Program memakai fungsi `is_contract_modified()` dimana fungsi ini:
1. membuka `contract.txt`
2. membaca baris pertama
3. membaca baris kedua
4. memeriksa apakah baris pertama masih sama dengan quote asli
5. memeriksa apakah baris kedua masih diawali oleh `created at:` atau `restored at:`.

Pengecekan modifikasi file dilakukan dengan perbandingan isi sebagai berikut:

```c
if (strcmp(line1, original_quote) != 0) {
    return 1;
}
if (strncmp(line2, "created at: ", 12) != 0 &&
    strncmp(line2, "restored at: ", 13) != 0) {
    return 1;
}
```

Di loop utama, saat file terdeteksi ada perubahan, program melakukan:

```c
write_violation_log();
restore_contract_file();
```

Artinya ada dua aksi sekaligus:
1. menulis `contract violated.` ke [`work.log`](soal_2/work.log)
2. mengembalikan [`contract.txt`](soal_2/contract.txt) ke bentuk yang benar. 

Fungsi `write_violation_log()`  menulis baris log seperti:

```text
[2026-04-08 02:35:10] contract violated.
```

#### 6. Saat daemon dihentikan, tulis pesan terakhir ke work.log

Supaya daemon bisa berhenti dengan rapi, kode memasang signal handler:

```c
signal(SIGTERM, stop_daemon);
signal(SIGINT, stop_daemon);
```

Artinya, kalau daemon menerima `SIGTERM` atau menerima `SIGINT` maka fungsi `stop_daemon()` akan dipanggil.

```c
void stop_daemon(int sig) {
    running = 0;
}
```

Jadi daemon tidak langsung berhenti mendadak, melainkan hanya mengubah `running` menjadi `0`. Karena loop utama berbentuk `while (running)`, loop akan berhenti dan membuat program punya kesempatan untuk menulis log penutup. Setelah keluar dari loop, program menjalankan:

```c
write_goodbye_log();
```

Fungsi ini membuka [`work.log`](soal_2/work.log), mengambil timestamp, dan menulis:

```text
[2026-04-08 02:40:00] We really weren’t meant to be together.
```

Jadi penghentian daemon tetap tercatat dengan rapi di log. Ini sesuai dengan poin terakhir soal. 

#### 7. Semua operasi dilakukan otomatis lewat program C

Semua operasi harus terjadi otomatis melalui program C, dapat dibuktikan dengan hal sebagai berikut;

- `contract.txt` dibuat oleh fungsi `create_contract_file()`
- `work.log` diisi oleh fungsi `write_log_message()`, `write_violation_log()`, dan `write_goodbye_log()`
- file dipulihkan otomatis oleh `restore_contract_file()`
- isi file dicek otomatis oleh `is_contract_modified()`
- penghentian daemon juga ditangani otomatis lewat signal handler. 

---

### Dokumentasi 

#### 1. Compile program untuk menjalankan daemon dan bukti daemon berjalan

![Compile program untuk menjalankan daemon dan bukti daemon berjalan](<assets/soal_2/1-1-compile dan cek grep contract_daemon.png>)

#### 2. Melihat isi contract.txt awal

![Melihat isi contract.txt awal](<assets/soal_2/1-2-cat awal.png>)

#### 3. Menguji penghapusan file dan restored

![Menguji penghapusan file](<assets/soal_2/1-3-cek hapus file.png>)

#### 4. Bukti perubahan isi file

![Bukti perubahan isi file](<assets/soal_2/1-4-cek nano rusak.png>)

#### 5. Isi work.log setelah ada perubahan isi file

![Isi work.log setelah ada perubahan isi file](<assets/soal_2/1-5-cek work.log violated.png>)

#### 6. Menghentikan daemon

![Menghentikan daemon](<assets/soal_2/1-6-cek kill PID.png>)

---
### Kendala

Tidak ada.

