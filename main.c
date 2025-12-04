#include <sys/mman.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "include/shared_data.h"

// Определения констант
#define SHM_NAME "/shm_lab3"
#define SEM_CHILD1_NAME "/sem_child1"
#define SEM_CHILD2_NAME "/sem_child2" 
#define SEM_PARENT_NAME "/sem_parent"

int main() {
    // Создаём объект shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Устанавливаем размер разделяемой памяти
    if (ftruncate(shm_fd, sizeof(shared_data)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Отображаем разделяемую память в адресное пространство процесса
    shared_data *shared_mem = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Инициализируем разделяемую память
    memset(shared_mem->buffer, 0, SHM_SIZE);
    shared_mem->size = 0;
    shared_mem->exit_flag = 0;

    // Создание семафоров для синхронизации
    sem_t *sem_child1 = sem_open(SEM_CHILD1_NAME, O_CREAT, 0666, 0);
    sem_t *sem_child2 = sem_open(SEM_CHILD2_NAME, O_CREAT, 0666, 0);
    sem_t *sem_parent = sem_open(SEM_PARENT_NAME, O_CREAT, 0666, 0);
    
    if (sem_child1 == SEM_FAILED || sem_child2 == SEM_FAILED || sem_parent == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Создание процессов
    pid_t pid1, pid2;

    // Создание первого дочернего процесса
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    // child1 process
    if (pid1 == 0) {
        execl("./child1", "child1", NULL);
        perror("execl error");
        exit(EXIT_FAILURE);
    }

    // Создание второго дочернего процесса
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    
    // child2 process
    if (pid2 == 0) {
        execl("./child2", "child2", NULL);
        perror("execl error");
        exit(EXIT_FAILURE);
    }

    // Родительский процесс
    printf("Программа для обработки строк (верхний регистр + удаление задвоенных пробелов)\n");
    printf("Введите строки для обработки (Ctrl+D для завершения):\n");

    char input_buffer[SHM_SIZE];
    
    while (1) {
        printf("Введите строку: ");
        fflush(stdout);
        
        // Чтение строки от пользователя
        if (fgets(input_buffer, SHM_SIZE, stdin) == NULL) {
            // Сигнал завершения (Ctrl+D)
            shared_mem->exit_flag = 1;
            sem_post(sem_child1);  // Будим child1 для завершения
            break;
        }

        // Убираем символ новой строки
        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        // Проверяем команду выхода
        if (strcmp(input_buffer, "quit") == 0) {
            shared_mem->exit_flag = 1;
            sem_post(sem_child1);  // Будим child1 для завершения
            break;
        }

        // Копируем данные в разделяемую память
        strncpy(shared_mem->buffer, input_buffer, SHM_SIZE - 1);
        shared_mem->buffer[SHM_SIZE - 1] = '\0';
        shared_mem->size = strlen(shared_mem->buffer) + 1;

        // Запускаем обработку
        sem_post(sem_child1);  // Разрешаем child1 читать из shm

        // Ждем завершения обработки
        sem_wait(sem_parent);  // Ждём, пока оба процесса закончат обработку

        // Выводим результат
        printf("Результат: %s\n", shared_mem->buffer);
    }

    // Ожидание завершения дочерних процессов
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Очистка ресурсов
    munmap(shared_mem, sizeof(shared_data));
    shm_unlink(SHM_NAME);
    sem_close(sem_child1);
    sem_close(sem_child2);
    sem_close(sem_parent);
    sem_unlink(SEM_CHILD1_NAME);
    sem_unlink(SEM_CHILD2_NAME);
    sem_unlink(SEM_PARENT_NAME);

    printf("Программа завершена.\n");
    return 0;
}