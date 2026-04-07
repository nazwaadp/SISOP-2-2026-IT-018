#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>// untuk fork, setsid, chdir, sleep
#include <sys/stat.h> // untuk umask, stat
#include <sys/types.h> // tipe data sistem
#include <time.h> // untuk waktu / timestamp
#include <string.h> // untuk strcmp, strlen
#include <fcntl.h> // untuk open
#include <signal.h> // untuk signal

// variabel global untuk menyimpan status program jalan atau tidak
int running = 1;

// isi contract.txt saat pertama kali dibuat
const char *original_quote = "“A promise to keep going, even when unseen.”\n";

void stop_daemon(int sig) { // dipakai kalau daemon dihentikan
    running = 0; // kalau kena sinyal stop, ubah running jadi 0 agar loop berhenti
}

void get_timestamp(char *buffer, size_t size) { // untuk mengambil waktu sekarang dalam format YYYY-MM-DD HH:MM:SS
    time_t now; // variabel untuk simpan waktu saat ini
    struct tm *time_info; // pointer ke struktur waktu lokal
    time(&now); // ambil waktu sekarang
    time_info = localtime(&now); // ubah ke waktu lokal

    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info); // formatnya
}

// fungsi untuk menulis log ke work.log
void write_log_message() {
    FILE *log_file; // file pointer untuk work.log
    char timestamp[100]; // buffer waktu
    int random_status; // angka acak untuk pilih status
    char *status_text; // teks status yang dipilih

    get_timestamp(timestamp, sizeof(timestamp)); // ambil waktu sekarang
    random_status = rand() % 3; // hasil acak 0, 1, atau 2
    if (random_status == 0) {
        status_text = "[awake]";
    } else if (random_status == 1) {
        status_text = "[drifting]";
    } else {
        status_text = "[numbness]";
    }

    log_file = fopen("work.log", "a"); // buka file log mode append
    if (log_file != NULL) {
        fprintf(log_file, "[%s] still working... %s\n", timestamp, status_text);
        // tulis log ke file
        fclose(log_file); // tutup file
    }
}

// fungsi untuk menulis "contract violated." ke work.log
void write_violation_log() {
    FILE *log_file;
    char timestamp[100];

    get_timestamp(timestamp, sizeof(timestamp)); // ambil waktu sekarang
    log_file = fopen("work.log", "a"); // buka log mode tambah
    if (log_file != NULL) {
        fprintf(log_file, "[%s] contract violated.\n", timestamp);
        fclose(log_file);
    }
}

void write_goodbye_log() { // fungsi untuk nulis pesan terakhir saat daemon berhenti
    FILE *log_file;
    char timestamp[100];

    get_timestamp(timestamp, sizeof(timestamp)); // ambil waktu sekarang
    log_file = fopen("work.log", "a"); // buka log
    if (log_file != NULL) {
        fprintf(log_file, "[%s] We really weren’t meant to be together.\n", timestamp);
        fclose(log_file);
    }
}

// fungsi untuk membuat contract.txt pertama kali
void create_contract_file() {
    FILE *contract_file; // pointer file contract.txt
    char timestamp[100]; // buffer waktu

    get_timestamp(timestamp, sizeof(timestamp)); // ambil waktu sekarang
    contract_file = fopen("contract.txt", "w"); // buka file mode write
    if (contract_file != NULL) {
        fprintf(contract_file, "%s", original_quote); // tulis quote di baris pertama
        fprintf(contract_file, "created at: %s\n", timestamp); // tulis waktu pembuatan di baris kedua
        fclose(contract_file); // tutup file
    }
}

// fungsi untuk restore contract.txt kalau file hilang
void restore_contract_file() {
    FILE *contract_file;
    char timestamp[100];

    get_timestamp(timestamp, sizeof(timestamp)); // ambil waktu sekarang
    contract_file = fopen("contract.txt", "w"); // buat ulang file
    if (contract_file != NULL) {
        fprintf(contract_file, "%s", original_quote); // tulis ulang quote asli
        fprintf(contract_file, "restored at: %s\n", timestamp); // baris kedua harus jadi restored at
        fclose(contract_file);
    }
}

// fungsi untuk mengecek apakah isi contract.txt masih sama atau tidak
int is_contract_modified() {
    FILE *contract_file; // pointer file
    char line1[300]; // buffer baris pertama
    char line2[300]; // buffer baris kedua

    contract_file = fopen("contract.txt", "r"); // buka file untuk dibaca
    if (contract_file == NULL) {
        return 0; // kalau file tidak ada, bukan kasus modify, tapi kasus deleted
    }

    if (fgets(line1, sizeof(line1), contract_file) == NULL) { // baca baris pertama
        fclose(contract_file);
        return 1; // kalau gagal baca, anggap file rusak / berubah
    }

    if (fgets(line2, sizeof(line2), contract_file) == NULL) { // baca baris kedua
        fclose(contract_file);
        return 1; // kalau baris kedua tidak ada, berarti isi file berubah
    }
    fclose(contract_file); // tutup file

    // bandingkan baris pertama dengan quote asli
    if (strcmp(line1, original_quote) != 0) {
        return 1; // kalau beda, berarti file diubah
    }

    // cek apakah baris kedua diawali "created at: " atau "restored at: "
    if (strncmp(line2, "created at: ", 12) != 0 &&
        strncmp(line2, "restored at: ", 13) != 0) {
        return 1; // kalau format baris kedua tidak sesuai, berarti diubah
    }

    return 0; // kalau aman, file tidak diubah
}

int main() {
    pid_t pid; // untuk menyimpan hasil fork
    struct stat file_info; // untuk mengecek file ada atau tidak

    srand(time(NULL)); // seed random supaya status acak tidak sama terus

    // pasang handler sinyal supaya saat dihentikan bisa tulis pesan terakhir
    signal(SIGTERM, stop_daemon);
    signal(SIGINT, stop_daemon);

    pid = fork(); // proses baru

    if (pid < 0) {
        exit(EXIT_FAILURE); // kalau fork gagal, keluar
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); // parent selesai, child lanjut jadi daemon
    }

    // membuat session baru agar lepas dari terminal
    if (setsid() < 0) {
        exit(EXIT_FAILURE); // kalau gagal, keluar
    }

    // fork kedua supaya daemon lebih stabil dan tidak jadi session leader
    pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE); // kalau fork gagal
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); // parent kedua keluar
    }

    umask(0); // reset permission mask

    chdir("."); // tetap di folder saat ini

    // tutup file descriptor standar
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // buat contract.txt saat program mulai
    create_contract_file();

    while (running) { // loop utama daemon
        write_log_message(); // tulis still working tiap 5 detik

        // cek adanya contract.txt
        if (stat("contract.txt", &file_info) != 0) {
            // kalau file tidak ada, restore
            sleep(1); // tunggu 1 detik biar sesuai soal 1-2 detik
            restore_contract_file();
        } else {
            // kalau ada, cek apakah isinya diubah
            if (is_contract_modified()) {
                write_violation_log(); // tulis contract violated.
                restore_contract_file(); // balikin isi file
            }
        }

        sleep(5); // tunggu 5 detik sebelum loop berikutnya
    }

    write_goodbye_log(); // kl loop selesai karena daemon dihentikan, tulis pesan terakhir

    return 0;
}
