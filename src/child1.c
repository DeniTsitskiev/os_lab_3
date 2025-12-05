#include <sys/mman.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/string_to_uppercase.h"
#include "../include/shared_data.h"

// Добавьте эти определения
#define SHM_SIZE 1024
#define SHM_NAME "/shm_lab3"
#define SEM_CHILD1_NAME "/sem_child1"
#define SEM_CHILD2_NAME "/sem_child2"
#define SEM_PARENT_NAME "/sem_parent"

int main() {
    // Открываем разделяемую память
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open in child1");
        exit(EXIT_FAILURE);
    }

    // Отображаем разделяемую память
    shared_data *shared_mem = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap in child1");
        exit(EXIT_FAILURE);
    }

    // Открываем семафоры
    sem_t *sem_child1 = sem_open(SEM_CHILD1_NAME, 0);
    sem_t *sem_child2 = sem_open(SEM_CHILD2_NAME, 0);
    sem_t *sem_parent = sem_open(SEM_PARENT_NAME, 0);
    
    if (sem_child1 == SEM_FAILED || sem_child2 == SEM_FAILED || sem_parent == SEM_FAILED) {
        perror("sem_open in child1");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Ждем сигнала от родителя
        sem_wait(sem_child1);

        // Проверяем флаг завершения
        if (shared_mem->exit_flag) {
            break;
        }

        // Обрабатываем строку (верхний регистр) - используем функцию из string_to_uppercase.c
        string_to_uppercase(shared_mem->buffer);

        // Передаем данные child2
        sem_post(sem_child2);
    }

    // Закрываем ресурсы
    munmap(shared_mem, sizeof(shared_data));
    close(shm_fd);
    sem_close(sem_child1);
    sem_close(sem_child2);
    sem_close(sem_parent);

    return 0;
}