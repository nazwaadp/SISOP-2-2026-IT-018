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
Soal ini meminta untuk membuat program C bernama `kasir_muthu.c` untuk membantu Uncle Muthu mengamankan data penting yang ada di file `buku_hutang.csv`.Pada soal diceritakan bahwa komputer kasir terkena virus, jadi  data-data di kasir harus segera diamankan sebelum terjadi hal yang tidak diinginkan.

Program harus dijalankan secara urut menggunakan konsep *Sequential Process*, yaitu parent process nanti membuat child process satu per satu untuk menjalankan tugas tertentu, lalu parent harus menunggu child tersebut selesai sebelum lanjut ke tugas berikutnya. Karena itu, soal ini secara khusus meminta penggunaan kombinasi:

1. `fork()`
2. `exec()`
3. `waitpid()`

Pada soal ini juga dilarang keras menggunakan `system()`, jadi semua command harus dijalankan melalui `exec()`.

Proses dalam program yang diminta dalam soal sebagai berikut: <br>
1. Membuat folder [`brankas_kedai`](soal_1/brankas_kedai)
2. Menyalin file `buku_hutang.csv` ke folder tersebut
3. Mencari semua data pelanggan dengan status "Belum Lunas"
4. Menyimpan hasil pencarian itu ke file `daftar_penunggak.txt`
5. Mengompres folder [`brankas_kedai](soal_1/brankas_kedai)` menjadi `rahasia_muthu.zip`
6. Jika ada satu proses yang gagal, program harus langsung berhenti
7. Jika semua berhasil, program menampilkan pesan sukses.
---

### Penyelesaian Soal
Program dibuat menggunakan library sebagai berikut:

- `stdio.h` untuk menampilkan output ke terminal
- `stdlib.h` untuk fungsi `exit()`
- `unistd.h` untuk `fork()` dan `execlp()`
- `sys/types.h` untuk tipe data `pid_t`
- `sys/wait.h` untuk `waitpid()` dan pengecekan status child process

Di dalam `main()`, ada dua variabel penting:
- `pid_t pid;` untuk menyimpan ID proses hasil `fork()`
- `int status;` untuk menyimpan status hasil eksekusi child process

Dimulai dari kode pada program yang digunakan untuk enghapus hasil lama jika masih ada. Hal ini dilakukan supaya saat program dijalankan ulang, folder dan file hasil percobaan sebelumnya tidak bentrok dengan hasil baru. <br>
Command yang dijalankan di awal adalah:

```bash
rm -rf brankas_kedai rahasia_muthu.zip
```
Langkah ini dilakukan dengan tujuan mempermudah juga program lebih rapi saat diuji berkali-kali.

#### 1. Program menggunakan sequential process dengan parent dan child process.

Pada soal poin 1, program ini dibuat dengan konsep sequential process. Artinya, semua proses berjalan berurutan, bukan bersamaan. <br>
Pola kerjanya sakni: <br>
1. parent process membuat child process dengan fork()
2. child process menjalankan command tertentu dengan execlp()
3. parent process menunggu child selesai menggunakan waitpid()
4. parent memeriksa apakah proses child berhasil atau gagal
5. kalau berhasil, parent lanjut ke langkah berikutnya
6. kalau gagal, program langsung berhenti

Bagian ini terlihat berulang di setiap langkah. 
```bash
pid = fork();
if (pid < 0) {
    printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(1);
}
```
`fork()`  dipakai untuk membuat proses baru
kalau hasil `fork()` kurang dari 0, maka proses child gagal dibuat. Jika gagal, program langsung menampilkan pesan error dan berhenti. Setelah itu, jika `pid == 0`, berarti bagian kode tersebut dijalankan oleh child process, sedangkan parent akan menunggu hasilnya. <br>
Fungsi `waitpid(pid, &status, 0)`; sangat penting karena parent tidak boleh lanjut sebelum child selesai. Ini yang membuat proses tetap berjalan urut, sesuai konsep sequential process.

#### 2. Child process pertama membuat folder brankas_kedai

Pada program, terdapat kode dimana parent membuat child process baru yakni `pid = fork();`.
Lalu jika child process berhasil dibuat dan `pid == 0`, child menjalankan: <br>
``` bash
execlp("mkdir", "mkdir", "brankas_kedai", NULL);
```
Tujuan langkah ini adalah membuat folder khusus yang akan dipakai sebagai tempat penyimpanan file penting. Jika `execlp()` gagal menjalankan command `mkdir`, maka child akan menampilkan pesan: <br>

``` bash
printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
exit(1);
```

Setelah child selesai, parent menunggu dengan `waitpid(pid, &status, 0);`, Lalu parent mengecek apakah proses benar-benar berhasil menggunakan kode berikut:

```bash
if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(1);
}
```

Jadi, folder `brankas_kedai` harus berhasil dibuat dulu. Jika gagal, program tidak boleh lanjut ke proses berikutnya.

#### 3. Child process kedua menyalin file buku_hutang.csv ke brankas_kedai

Setelah folder berhasil dibuat, parent membuat child process kedua untuk menyalin file `buku_hutang.csv` ke dalam folder `brankas_kedai.`<br>
Pada kode, child menjalankan:

``` bash
execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
```

Perintah ini setara dengan command:

``` bash
cp buku_hutang.csv brankas_kedai/
```

Tujuan langkah ini adalah mengamankan file asli dengan membuat salinannya di dalam folder brankas. Langkah ini harus dilakukan setelah `mkdir` karena file tujuan akan disalin ke folder `brankas_kedai`, jadi foldernya harus sudah ada terlebih dahulu. Inilah alasan kenapa program harus berjalan berurutan.

Setelah child selesai menjalankan cp, parent kembali menunggu: <br>
`waitpid(pid, &status, 0);` <br>
Kemudian statusnya diperiksa. Jika file `buku_hutang.csv` tidak ada, atau penyalinan gagal, maka status proses tidak akan bernilai sukses dan program langsung berhenti.

Jadi, tahap ini memastikan bahwa file penting sudah berhasil dipindahkan ke lokasi aman sebelum diproses lebih lanjut.

#### 4. Child process ketiga mencari data dengan status "Belum Lunas" lalu menyimpannya ke daftar_penunggak.txt

Setelah file CSV berhasil masuk ke dalam folder `brankas_kedai/`, parent membuat child process ketiga untuk mencari seluruh data pelanggan yang masih memiliki status utang "Belum Lunas".

Pada kode, child menjalankan:

``` bash
execlp("sh", "sh", "-c",
       "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
       NULL);
```

Perintah ini setara dengan:

```bash
grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt
```
Penjelasan singkat kode diatas : grep 'Belum Lunas' dipakai untuk mencari semua baris yang mengandung teks Belum Lunas dan file yang dibaca adalah `brankas_kedai/buku_hutang.csv`
hasil pencarian disimpan ke file `brankas_kedai/daftar_penunggak.txt`

Di sini digunakan `sh -c` karena command mengandung tanda `>`, yaitu **redirect output ke file**. Redirect seperti ini tidak bisa dipakai langsung kalau hanya memanggil grep biasa lewat `execlp()`. Karena itu, command dibungkus lewat shell. <br>
Langkah ini penting karena soal tidak hanya meminta file CSV diamankan, tetapi juga meminta data pelanggan yang belum melunasi hutang dipisahkan ke file tersendiri.

Setelah proses grep selesai, parent menunggu lagi dengan `waitpid()` lalu memeriksa statusnya. Jika gagal, program langsung berhenti dan tidak lanjut ke tahap ke-5.

#### 5. Child process keempat mengompres folder menjadi rahasia_muthu.zip

Setelah folder `brankas_kedai/` berisi file `buku_hutang.csv` dan `daftar_penunggak.txt`, parent membuat child process keempat untuk mengompres folder tersebut menjadi file ZIP.<br>
Pada kode, child menjalankan:

``` bash
execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
```

Perintah ini sama seperti:

```bash
zip -r rahasia_muthu.zip brankas_kedai
```

Penjelasan singkat kode diatas : zip digunakan untuk membuat file kompresi dan `-r` berarti folder dikompres secara **rekursif**, termasuk semua isi di dalamnya. `rahasia_muthu.zip` adalah nama file output
`brankas_kedai` adalah folder yang akan dikompres

Tujuan langkah ini adalah mengamankan seluruh isi folder dalam satu file arsip, sehingga data lebih mudah dipindahkan dan disimpan. Setelah child selesai menjalankan zip, parent menunggu lagi dan mengecek hasilnya. Jika proses kompres gagal, program juga langsung berhenti.

#### 6. Setiap proses dicek status keberhasilannya, dan program berhenti jika ada yang gagal

Hal penting dalam soal ini adalah program harus langsung berhenti jika ada langkah yang gagal. Pada kode, hal ini diterapkan pada setiap proses setelah `waitpid().` <br>
Bagian yang dipakai untuk pengecekan adalah:

```bash
if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(1);
}
```

Penjelasan singkat kode tersebut : `WIFEXITED(status) mengecek apakah child process selesai dengan normal, sedangkan `WEXITSTATUS(status)` mengambil nilai exit status dari child. Jika exit status tidak sama dengan 0, berarti proses dianggap gagal

Jadi, walaupun `fork()` berhasil, belum tentu command di dalam child juga berhasil. Karena itu, hasil eksekusi command harus tetap dicek.

Contohnya ketika `file buku_hutang.csv` tidak ada, proses `cp` akan gagal, jika folder belum terbentuk, proses selanjutnya juga bisa gagal, begitupun command zip tidak tersedia, proses kompres akan gagal

Jika salah satu dari hal itu terjadi, program menampilkan error:
``` bash
[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.
```
Setelah itu program langsung berhenti dengan `exit(1);.`

Dengan cara ini, kode sudah memenuhi syarat soal bahwa program tidak boleh melanjutkan proses berikutnya kalau ada langkah yang gagal.

#### 7. Jika semua langkah berhasil, program menampilkan pesan info

Ketika semua proses dari awal sampai akhir berhasil, maka di bagian terakhir program akan menampilkan pesan:

```bash
printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");
```
Jadi, jika pesan diatas muncul, berarti seluruh alur pengamanan data sudah selesai tanpa kendala dan menandakan bahwa: <br>
- Folder `brankas_kedai/` berhasil dibuat
- File `buku_hutang.csv` berhasil disalin
- Data pelanggan yang "Belum Lunas" berhasil dipisahkan
- File `daftar_penunggak.txt` berhasil dibuat
f- Folder berhasil dikompres menjadi `rahasia_muthu.zip`

### Dokumentasi
sini
### Kendala
Tidak ada.


## Soal 2 - THE WORLD NEVER STOPS, EVEN WHEN YOU FEEL TIRED.

### Penjelasan Soal
Pada soal ini, kita diminta membuat sebuah program C bernama `contract_daemon.c` yang berjalan sebagai **daemon**. Daemon adalah program yang berjalan terus di background tanpa harus terus terlihat di terminal. Jadi setelah program dijalankan, prosesnya tetap aktif dan tetap bekerja sendiri di belakang layar.

Di soal ini, daemon yang dibuat punya tugas utama untuk menjaga dua hal, yaitu:
1. terus menulis aktivitas kerja ke file `work.log`
2. menjaga agar file `contract.txt` selalu ada dan selalu berada dalam kondisi yang benar

Jadi program ini bukan program sekali jalan yang langsung selesai, melainkan program yang terus hidup, terus mengecek kondisi file, dan terus bertindak kalau ada perubahan tertentu.

Sesuai perintah soal, ada beberapa requirement utama yang harus dipenuhi.

#### 1. Membuat file `contract_daemon.c`
Program harus ditulis di file bernama `contract_daemon.c`. File ini menjadi pusat seluruh logika program.

#### 2. Daemon harus menulis log ke `work.log` setiap 5 detik
Setiap 5 detik sekali, daemon harus menambahkan satu baris log ke file `work.log`. Isi lognya harus berbentuk:

```text
still working… [status]
````

dengan status acak dari tiga kemungkinan berikut:

* `[awake]`
* `[drifting]`
* `[numbness]`

Artinya, selama daemon masih hidup, file `work.log` akan terus bertambah isinya.

#### 3. Saat program mulai berjalan, harus membuat `contract.txt`

Begitu daemon mulai aktif, program harus langsung membuat file `contract.txt`. Isi file tersebut harus memiliki dua baris:

* baris pertama: kutipan tetap
* baris kedua: waktu pembuatan file dengan format `created at: <timestamp>`

#### 4. Jika `contract.txt` dihapus, file harus dibuat kembali

Kalau file `contract.txt` dihapus, daemon tidak boleh diam. File itu harus dibuat ulang dalam waktu sekitar 1–2 detik. Saat file dibuat ulang, isi baris pertamanya tetap sama, tetapi baris keduanya berubah menjadi:

```text
restored at: <timestamp>
```

Jadi program harus bisa membedakan mana kondisi file normal, mana kondisi file hilang.

#### 5. Jika isi `contract.txt` diubah, daemon harus menganggap kontrak dilanggar

Kalau file `contract.txt` tidak dihapus tapi isinya diubah, daemon harus:

* menulis `contract violated.` ke `work.log`
* mengembalikan isi `contract.txt` ke bentuk semula

Jadi daemon bukan cuma menjaga keberadaan file, tapi juga menjaga **isi** file.

#### 6. Jika daemon dihentikan, tulis pesan terakhir ke `work.log`

Saat proses daemon dihentikan, program harus menuliskan pesan:

```text
We really weren’t meant to be together
```

ke dalam `work.log`.

#### 7. Semua operasi harus dilakukan lewat program C

Soal menegaskan bahwa semua proses ini harus ditangani oleh program C, bukan dibuat manual. Artinya:

* `contract.txt` tidak dibuat manual dari luar
* `work.log` tidak diisi manual
* restore file tidak dilakukan manual
* isi file yang salah harus diperbaiki otomatis oleh daemon

Intinya, program ini harus benar-benar menjadi penjaga otomatis yang terus bekerja sendiri di background. Requirement-requirement tersebut juga tampak jelas pada deskripsi soal dan hasil pengujian yang kamu kirim.  

---

### Penyelesaian Soal

Pada penyelesaian ini, program dibuat dengan konsep daemon yang terus berjalan di background. Supaya program bisa melakukan itu, dibutuhkan beberapa library standar C dan sistem operasi Linux, yaitu:

* `stdio.h`
* `stdlib.h`
* `unistd.h`
* `sys/stat.h`
* `sys/types.h`
* `time.h`
* `string.h`
* `fcntl.h`
* `signal.h` 

Masing-masing library dipakai untuk keperluan tertentu:

* `stdio.h` dipakai untuk `FILE`, `fopen`, `fprintf`, `fgets`, dan `fclose`
* `stdlib.h` dipakai untuk `exit`, `rand`, dan `srand`
* `unistd.h` dipakai untuk `fork`, `setsid`, `chdir`, `sleep`, dan `close`
* `sys/stat.h` dipakai untuk `stat` dan `umask`
* `time.h` dipakai untuk pengambilan timestamp
* `string.h` dipakai untuk `strcmp` dan `strncmp`
* `signal.h` dipakai untuk menangani sinyal saat daemon dihentikan. Semua ini terlihat langsung di source code yang kamu kirim. 

Di awal kode, ada dua bagian dasar yang sangat penting:

```c
int running = 1;
const char *original_quote = "“A promise to keep going, even when unseen.”\n";
```

`running` adalah penanda apakah loop daemon masih terus jalan atau harus berhenti.
`original_quote` adalah isi asli baris pertama `contract.txt` yang akan dipakai terus saat file dibuat pertama kali maupun saat dipulihkan lagi. 

---

#### 1. Program dibuat dalam file `contract_daemon.c`

Requirement pertama soal sebenarnya adalah titik awal pengerjaan, yaitu membuat file bernama `contract_daemon.c`. Dari sisi implementasi, file inilah yang memuat semua logika daemon.

Di dalam file ini, struktur program dibagi menjadi:

* variabel global
* fungsi handler sinyal
* fungsi pembuat timestamp
* fungsi penulis log rutin
* fungsi penulis log pelanggaran
* fungsi penulis log perpisahan
* fungsi pembuat file contract
* fungsi pemulih file contract
* fungsi pengecek apakah isi file berubah
* fungsi `main()` yang menjadi pusat daemonisasi dan loop utama. Struktur ini membuat kode lebih rapi karena setiap tugas punya fungsi masing-masing, tidak ditumpuk semua di dalam `main()`.  

---

#### 2. Daemon menulis `still working...` ke `work.log` setiap 5 detik

Requirement kedua meminta daemon terus menulis log setiap 5 detik. Pada kode, tugas ini dikerjakan oleh fungsi `write_log_message()`.

Di dalam fungsi itu, langkah-langkahnya adalah:

1. mengambil waktu sekarang
2. memilih angka acak dari `0` sampai `2`
3. mengubah angka itu menjadi salah satu status
4. membuka file `work.log` dengan mode append
5. menulis satu baris log
6. menutup file lagi. Semua langkah ini terlihat di fungsi `write_log_message()`. 

Bagian pentingnya adalah:

```c
random_status = rand() % 3;
```

Artinya, angka acak yang mungkin muncul hanya:

* `0`
* `1`
* `2`

Lalu angka itu diterjemahkan seperti ini:

* `0` → `[awake]`
* `1` → `[drifting]`
* `2` → `[numbness]`

Setelah status dipilih, file `work.log` dibuka dengan:

```c
log_file = fopen("work.log", "a");
```

Mode `"a"` berarti append, jadi isi lama tidak dihapus. Log baru selalu ditambahkan di bagian bawah file. Setelah itu ditulis:

```c
fprintf(log_file, "[%s] still working... %s\n", timestamp, status_text);
```

Jadi format log akhirnya menjadi seperti:

```text
[2026-04-08 02:30:29] still working... [awake]
```

Supaya hasil acaknya tidak selalu sama, di dalam `main()` ada:

```c
srand(time(NULL));
```

yang berfungsi sebagai seed random berdasarkan waktu saat program mulai. 

Fungsi `write_log_message()` ini dipanggil terus di dalam loop utama:

```c
while (running) {
    write_log_message();
    ...
    sleep(5);
}
```

Karena ada `sleep(5)`, maka satu putaran loop berlangsung setiap 5 detik sekali. Inilah yang membuat `work.log` terus bertambah dengan jeda waktu tetap. Dari hasil `cat work.log` yang kamu kirim, memang terlihat banyak entri log berurutan dengan status acak yang berubah-ubah, jadi bagian ini berjalan sesuai permintaan soal. 

---

#### 3. Saat program mulai berjalan, daemon membuat `contract.txt`

Requirement ketiga meminta file `contract.txt` dibuat otomatis saat daemon mulai. Di kode, ini dikerjakan oleh fungsi `create_contract_file()`.

Alur fungsi ini:

1. mengambil timestamp sekarang
2. membuka `contract.txt` dengan mode `"w"`
3. menulis quote asli ke baris pertama
4. menulis `created at: <timestamp>` ke baris kedua
5. menutup file. Implementasi ini terlihat jelas di fungsi `create_contract_file()`. 

Isi pentingnya adalah:

* baris pertama selalu berasal dari `original_quote`
* baris kedua memakai format `created at: ...`

Pemanggilan fungsi ini dilakukan di `main()` setelah proses daemonisasi selesai:

```c
create_contract_file();
```

Artinya, begitu daemon sudah resmi aktif di background, file `contract.txt` langsung dibuat.

Dari screenshot terminal yang kamu kirim, setelah `./contract.daemon` dijalankan, perintah `ls` dan `tree` menunjukkan bahwa `contract.txt` dan `work.log` langsung muncul. Saat `cat contract.txt` dijalankan, isi file memang menunjukkan quote asli di baris pertama dan `created at:` di baris kedua. Jadi requirement ini terpenuhi.  

---

#### 4. Jika `contract.txt` dihapus, file dibuat kembali dalam 1–2 detik

Requirement keempat meminta daemon memulihkan `contract.txt` jika file itu hilang. Di kode, pengecekan keberadaan file dilakukan dengan:

```c
if (stat("contract.txt", &file_info) != 0)
```

Fungsi `stat()` dipakai untuk mengecek apakah file ada. Kalau return-nya bukan `0`, berarti file tidak ditemukan atau tidak bisa diakses. Di dalam loop utama, kalau kondisi ini terpenuhi, maka daemon melakukan:

```c
sleep(1);
restore_contract_file();
```

Jadi program sengaja menunggu 1 detik dulu, lalu memulihkan file. Ini masih sesuai requirement soal yang meminta pemulihan dalam waktu 1–2 detik. 

Fungsi `restore_contract_file()` isinya mirip dengan fungsi pembuatan awal, tetapi ada perbedaan penting pada baris kedua. Saat file dipulihkan, program menulis:

```text
restored at: <timestamp>
```

bukan `created at:` lagi. Jadi daemon bisa membedakan antara file yang baru dibuat pertama kali dan file yang dibuat ulang setelah hilang. Implementasi ini terlihat langsung di fungsi `restore_contract_file()`. 

Dari pengujian yang kamu lakukan:

* `rm contract.txt`
* lalu dicek lagi dengan `ls`
* lalu dibaca dengan `cat contract.txt`

hasilnya file muncul kembali, dan isi baris keduanya berubah menjadi `restored at: ...`. Itu membuktikan bahwa mekanisme restore berjalan sesuai soal. 

---

#### 5. Jika isi `contract.txt` diubah, daemon menulis `contract violated.` lalu mengembalikannya

Requirement kelima lebih rumit, karena di sini daemon harus mengecek **isi file**, bukan sekadar ada atau tidak.

Untuk menangani ini, kode memakai fungsi `is_contract_modified()`. Fungsi ini:

1. membuka `contract.txt`
2. membaca baris pertama
3. membaca baris kedua
4. memeriksa apakah baris pertama masih sama dengan quote asli
5. memeriksa apakah baris kedua masih diawali oleh `created at:` atau `restored at:`. Semua langkah ini ada di fungsi `is_contract_modified()`. 

Kalau file gagal dibuka, fungsi mengembalikan `0` karena kasus itu dianggap sebagai file hilang, bukan file diubah. Tetapi kalau file bisa dibuka lalu:

* baris pertama berbeda
* atau baris kedua tidak sesuai format
* atau baris kedua tidak ada
* atau isi file rusak

maka fungsi mengembalikan `1`, artinya file dianggap telah dimodifikasi.

Pengecekannya dilakukan dengan:

```c
if (strcmp(line1, original_quote) != 0) {
    return 1;
}
if (strncmp(line2, "created at: ", 12) != 0 &&
    strncmp(line2, "restored at: ", 13) != 0) {
    return 1;
}
```

Jadi program memang membandingkan isi file dengan bentuk yang seharusnya. Kalau tidak cocok, berarti kontrak dilanggar. 

Di loop utama, saat file terdeteksi berubah, program melakukan:

```c
write_violation_log();
restore_contract_file();
```

Artinya ada dua aksi sekaligus:

1. menulis `contract violated.` ke `work.log`
2. mengembalikan `contract.txt` ke bentuk yang benar. Implementasi ini sesuai dengan requirement soal. 

Fungsi `write_violation_log()` sendiri menulis baris log seperti:

```text
[2026-04-08 02:35:10] contract violated.
```

Karena file `contract.txt` kemudian langsung dipulihkan, maka saat kamu mengedit file dengan `nano contract.txt`, hasil akhirnya ketika dicek ulang lewat `cat contract.txt` seharusnya isi file kembali seperti semula. Sementara `cat work.log` akan menunjukkan bahwa pelanggaran tercatat. Alur tes yang kamu pakai memang tepat untuk membuktikan behaviour ini. 

---

#### 6. Saat daemon dihentikan, tulis pesan terakhir ke `work.log`

Requirement keenam meminta saat proses daemon berhenti, harus ada satu pesan terakhir di `work.log`.

Supaya daemon bisa berhenti dengan rapi, kode memasang signal handler:

```c
signal(SIGTERM, stop_daemon);
signal(SIGINT, stop_daemon);
```

Artinya:

* kalau daemon menerima `SIGTERM`
* atau menerima `SIGINT`

maka fungsi `stop_daemon()` akan dipanggil. Fungsi ini sangat sederhana:

```c
void stop_daemon(int sig) {
    running = 0;
}
```

Jadi daemon tidak langsung berhenti mendadak, melainkan hanya mengubah `running` menjadi `0`. Karena loop utama berbentuk `while (running)`, loop akan berhenti secara alami pada iterasi berikutnya. Ini membuat program punya kesempatan untuk menulis log penutup. Semua ini terlihat jelas di source code. 

Setelah keluar dari loop, program menjalankan:

```c
write_goodbye_log();
```

Fungsi ini membuka `work.log`, mengambil timestamp, lalu menulis:

```text
[2026-04-08 02:40:00] We really weren’t meant to be together.
```

Jadi penghentian daemon tetap tercatat dengan rapi di log. Ini sesuai dengan poin terakhir soal. 

---

#### 7. Semua operasi dilakukan otomatis lewat program C

Salah satu poin penting di soal adalah semua operasi harus terjadi otomatis lewat program C. Kode yang kamu buat memang sudah memenuhi ini karena:

* `contract.txt` dibuat oleh fungsi `create_contract_file()`
* `work.log` diisi oleh fungsi `write_log_message()`, `write_violation_log()`, dan `write_goodbye_log()`
* file dipulihkan otomatis oleh `restore_contract_file()`
* isi file dicek otomatis oleh `is_contract_modified()`
* penghentian daemon juga ditangani otomatis lewat signal handler. Semua ini ada langsung di source code, jadi tidak mengandalkan pembuatan file manual dari luar.

---

### Penjelasan Cara Kerja Fungsi-Fungsi pada Kode

#### Fungsi `stop_daemon(int sig)`

Fungsi ini dipakai saat daemon menerima sinyal penghentian. Tugasnya sederhana, yaitu mengubah `running` menjadi `0`. Dengan begitu loop utama berhenti, lalu program bisa masuk ke langkah penutup dan menulis pesan goodbye ke `work.log`. Jadi fungsi ini penting supaya daemon tidak mati mendadak. 

#### Fungsi `get_timestamp(char *buffer, size_t size)`

Fungsi ini dipakai hampir di semua bagian kode yang membutuhkan waktu. Alurnya:

* ambil waktu saat ini dengan `time(&now)`
* ubah ke waktu lokal dengan `localtime(&now)`
* format jadi string `YYYY-MM-DD HH:MM:SS` dengan `strftime()`
* simpan hasilnya ke buffer yang dikirim. Karena banyak fitur di soal butuh timestamp, fungsi ini dipakai berulang di log, pembuatan contract, restore, dan goodbye log. 

#### Fungsi `write_log_message()`

Fungsi ini membuat log kerja rutin. Setiap dipanggil, fungsi:

* ambil timestamp
* pilih satu status acak
* buka `work.log` mode append
* tulis satu baris `still working...`
* tutup file. Ini adalah fungsi utama yang membuat daemon terlihat “tetap hidup”. 

#### Fungsi `write_violation_log()`

Fungsi ini hanya dipanggil saat isi `contract.txt` terdeteksi berubah. Tugasnya hanya satu: menulis `contract violated.` ke `work.log` dengan timestamp. Jadi ini menjadi jejak bahwa ada pelanggaran pada isi file. 

#### Fungsi `write_goodbye_log()`

Fungsi ini dipanggil saat loop daemon selesai. Isi log yang ditulis adalah kalimat perpisahan sesuai yang diminta soal. Jadi ini menjadi tanda bahwa daemon berhenti secara normal, bukan crash. 

#### Fungsi `create_contract_file()`

Fungsi ini dipakai di awal program. Tugasnya membuat `contract.txt` pertama kali dengan dua baris:

* quote asli
* `created at: <timestamp>`. Fungsi ini hanya dipanggil sekali saat daemon baru mulai aktif. 

#### Fungsi `restore_contract_file()`

Fungsi ini dipakai kalau file hilang atau isi file rusak. Tugasnya mengembalikan file ke bentuk yang benar, tetapi dengan baris kedua:

* `restored at: <timestamp>`. Jadi fungsi ini tidak hanya membuat file, tetapi juga memberi penanda bahwa file tersebut adalah hasil pemulihan. 

#### Fungsi `is_contract_modified()`

Fungsi ini menjadi inti penjagaan isi file. Fungsi ini tidak sekadar mengecek file ada, tetapi benar-benar membaca isi dua baris pertama dan membandingkannya dengan bentuk yang seharusnya. Kalau ada yang tidak cocok, fungsi mengembalikan `1`. Kalau masih normal, fungsi mengembalikan `0`. Inilah yang membuat daemon bisa membedakan antara file normal, file hilang, dan file yang isinya diubah. 

---

### Penjelasan Alur Program dari Awal Sampai Berhenti

#### 1. Program dijalankan

Saat `./contract.daemon` dijalankan, program mulai dari `main()`.

#### 2. Random diinisialisasi

`rand()` diberi seed lewat `srand(time(NULL))` supaya status acak tidak selalu menghasilkan pola yang sama.

#### 3. Signal handler dipasang

Program memasang `SIGTERM` dan `SIGINT` supaya saat dihentikan, daemon tidak langsung mati, tetapi bisa menutup loop dengan rapi.

#### 4. Fork pertama

`fork()` pertama memisahkan parent dan child. Parent langsung keluar, child melanjutkan.

#### 5. `setsid()`

Child membuat session baru agar terlepas dari terminal dan berubah menjadi proses daemon.

#### 6. Fork kedua

Dilakukan fork kedua agar proses final bukan session leader. Ini adalah pola umum daemonisasi supaya lebih stabil.

#### 7. Pengaturan lingkungan daemon

Program menjalankan `umask(0)`, tetap di direktori saat ini dengan `chdir(".")`, lalu menutup `stdin`, `stdout`, dan `stderr`.

#### 8. Buat `contract.txt`

Setelah resmi menjadi daemon, program langsung memanggil `create_contract_file()`.

#### 9. Masuk ke loop utama

Selama `running` masih `1`, daemon terus:

* menulis `still working...`
* mengecek apakah `contract.txt` ada
* kalau hilang, restore
* kalau ada, cek apakah isinya diubah
* kalau diubah, tulis violation log dan restore
* tidur 5 detik

#### 10. Menerima sinyal stop

Saat user menjalankan `kill PID`, daemon menerima sinyal dan `running` diubah menjadi `0`.

#### 11. Keluar dari loop dan menulis goodbye log

Setelah loop selesai, program menulis pesan perpisahan ke `work.log`, lalu program benar-benar berakhir. Semua alur ini memang sesuai dengan struktur `main()` pada kode yang kamu kirim.

---

### Dokumentasi 

#### 1. Masuk ke folder pengerjaan

```bash
cd ~/IT2/SISOP/SISOP-2-2026-IT-018/soal_2
```

#### 2. Melihat isi folder sebelum compile

```bash
ls
```

Dari screenshot, sebelum compile sudah ada file source `contract.daemon.c`. 

#### 3. Compile program

```bash
gcc contract.daemon.c -o contract.daemon
```

Perintah ini mengubah source code C menjadi file executable bernama `contract.daemon`.

#### 4. Menjalankan daemon

```bash
./contract.daemon
```

Setelah command ini dijalankan, terminal memang terlihat seperti langsung kembali normal karena prosesnya berubah menjadi daemon dan jalan di background.

#### 5. Mengecek file hasil awal

```bash
ls
tree
```

Hasilnya menunjukkan:

* `contract.daemon`
* `contract.daemon.c`
* `contract.txt`
* `work.log`

Ini membuktikan bahwa saat daemon mulai, `contract.txt` dan `work.log` langsung dibuat. Hasil tersebut sesuai screenshot yang kamu kirim. 

#### 6. Mengecek apakah daemon benar-benar berjalan

```bash
ps aux | grep contract.daemon
```

Perintah ini dipakai untuk melihat proses daemon yang sedang aktif dan mendapatkan PID-nya.

#### 7. Melihat isi `contract.txt`

```bash
cat contract.txt
```

Dari hasil pengujianmu, isi file menampilkan quote asli dan baris `created at: ...`. Itu berarti pembuatan file awal berhasil. 

#### 8. Melihat isi `work.log`

```bash
cat work.log
```

Sempat ada salah ketik `work.log` langsung tanpa `cat`, sehingga terminal menganggap itu command. Setelah itu baru dicek benar dengan `cat work.log`. Itu tidak masalah, dan hasil akhirnya tetap menunjukkan isi log yang benar. 

#### 9. Menguji penghapusan file

```bash
rm contract.txt
ls
cat contract.txt
```

Tujuan tes ini adalah membuktikan bahwa kalau file dihapus, daemon akan membuatnya kembali secara otomatis.

#### 10. Menguji perubahan isi file

```bash
nano contract.txt
cat contract.txt
cat work.log
```

Di sini file sengaja diedit manual untuk melihat apakah daemon mendeteksi perubahan isi, lalu menulis `contract violated.` ke log dan memulihkan file.

#### 11. Menghentikan daemon

```bash
ps aux | grep contract.daemon
kill PID
cat work.log
```

Langkah ini dipakai untuk memastikan bahwa setelah daemon dihentikan, ada pesan perpisahan yang ditulis ke `work.log`.

---

### Dokumentasi Output

#### 1. Output setelah daemon pertama kali dijalankan

Berdasarkan hasil `ls` dan `tree`, setelah `./contract.daemon` dijalankan, file yang muncul adalah:

* `contract.daemon`
* `contract.daemon.c`
* `contract.txt`
* `work.log`

Ini menunjukkan bahwa daemon langsung membuat file contract dan log saat start. 

#### 2. Isi awal `contract.txt`

Saat dibaca dengan `cat contract.txt`, isi file menunjukkan:

* baris pertama: quote asli
* baris kedua: `created at: 2026-04-08 02:30:29`

Ini berarti fungsi `create_contract_file()` berjalan dengan benar pada saat daemon mulai. 

#### 3. Isi `work.log` saat daemon berjalan

Saat dibaca dengan `cat work.log`, isi file berisi banyak baris seperti:

* `[2026-04-08 02:30:29] still working... [awake]`
* `[2026-04-08 02:30:34] still working... [drifting]`
* `[2026-04-08 02:30:39] still working... [numbness]`

dan seterusnya. Ini membuktikan bahwa daemon benar-benar aktif dan menulis log terus-menerus dengan interval 5 detik serta status acak. 

#### 4. Output saat `contract.txt` dihapus

Setelah `rm contract.txt`, file tersebut hilang sesaat lalu muncul lagi. Saat dibaca ulang dengan `cat contract.txt`, isinya berubah menjadi:

* baris pertama tetap quote asli
* baris kedua menjadi `restored at: 2026-04-08 02:32:29`

Ini membuktikan daemon berhasil memulihkan file yang dihapus. 

#### 5. Output saat isi `contract.txt` diubah

Skenario ini diuji dengan mengedit file lewat `nano contract.txt`, lalu dicek lagi dengan `cat contract.txt` dan `cat work.log`. Berdasarkan logika kode, jika isi file berbeda dari format yang seharusnya, daemon akan:

* menulis `contract violated.` ke `work.log`
* memulihkan isi file kembali ke format benar. Mekanisme ini sesuai langsung dengan fungsi `is_contract_modified()`, `write_violation_log()`, dan `restore_contract_file()`.

#### 6. Output saat daemon dihentikan

Setelah PID daemon ditemukan lewat `ps aux | grep contract.daemon`, proses dihentikan dengan `kill PID`. Berdasarkan alur kode, penghentian ini membuat `running = 0`, loop berhenti, lalu `write_goodbye_log()` menulis pesan:
`We really weren’t meant to be together.`
ke `work.log`. Jadi log terakhir menjadi penanda bahwa daemon berakhir dengan rapi. 

---
### Kendala
Tidak ada.

