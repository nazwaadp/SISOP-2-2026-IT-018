#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> // untuk fork() dan execlp()
#include <sys/types.h> // untuk tipe data pid_t
#include <sys/wait.h> // untuk waitpid() dan cek status proses

int main() {
    pid_t pid; // variabel nyimpen id proses child
    int status; // variabel yg nyimpen status hasil proses child

    // Hapus hasil lama kalau ada
    pid = fork(); //buat proses baru (child) untuk menjalankan perintah rm
    if (pid < 0) { // kalau fork gagal dibuat
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1); //berhenti krn gagal
    }

    if (pid == 0) { // ini proses child
        execlp("rm", "rm", "-rf", "brankas_kedai", "rahasia_muthu.zip", NULL); // child menjalankan rm -rf untuk hapus folder dan zip lama
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n"); // tampilan kl execlp gagal
        exit(1); // child berhenti
    }

    waitpid(pid, &status, 0); // parent nunggu child selesai dulu

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { // cek child selesai gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    // Proses 1: mkdir
    pid = fork(); // buat child baru untuk mkdir
    if (pid < 0) { // kalau fork gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1); 
    } if (pid == 0) { // kl ini child
        execlp("mkdir", "mkdir", "brankas_kedai", NULL); // child mkdir brankas_kedai
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n"); // kl execlp gagal
        exit(1);
    }
    waitpid(pid, &status, 0);// parent nunggu child selesai
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { // cek proses mkdir gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    // Proses 2: cp
    pid = fork(); //buat child baru utk copy file
    if (pid < 0) { // kalau fork gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    } if (pid == 0) {// kl ini child
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL); // child jalanin cp untuk menyalin csv ke folder brankas
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n"); // kl execlp gagal
        exit(1);
    }
    waitpid(pid, &status, 0);   // parent nunggu child selesai
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { // cek proses copy gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1); 
    }

    // Proses 3: grep
    pid = fork(); // buat child baru untuk mencari data Belum Lunas
    if (pid < 0) {  // kl fork gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    } if (pid == 0) { // kl ini child
        execlp("sh", "sh", "-c", // pake shell karena ada tanda > untuk redirect output ke file
               "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt", //nyimpen yang blm lunas ke txt
               NULL); 
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n"); // kalau execlp gagal
        exit(1);
    }
    waitpid(pid, &status, 0);   // parent nunggu child selesai
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { // cek apakah proses grep gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1); 
    }

    // Proses 4: zip
    pid = fork(); // bikin child baru utk langkah kompres folder
    if (pid < 0) {  // kalau fork gagal
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1); 
    } if (pid == 0) { // kl ini child
        execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL); // child jalanin zip -r untuk kompres folder
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n"); // kl execlp gagal
        exit(1);
    }
    waitpid(pid, &status, 0);   // parent nunggu child selesai
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { // cek proses zip gagal 
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1); 
    }

    printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n"); // kalau semua langkah berhasil, kasi sukses

    return 0;  
}
