#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    // Hapus hasil lama kalau ada
    pid = fork();

    if (pid < 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("rm", "rm", "-rf", "brankas_kedai", "rahasia_muthu.zip", NULL);
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    // Proses 1: mkdir
    pid = fork();

    if (pid < 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("mkdir", "mkdir", "brankas_kedai", NULL);
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    // Proses 2: cp
    pid = fork();

    if (pid < 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    // Proses 3: grep
    pid = fork();

    if (pid < 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("sh", "sh", "-c",
               "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
               NULL);
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    // Proses 4: zip
    pid = fork();

    if (pid < 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(1);
    }

    printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");

    return 0;
}
