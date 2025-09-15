#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CHARS_PER_THREAD 100
#define NUM_THREADS 4
#define TOTAL_CHARS CHARS_PER_THREAD * NUM_THREADS
#define MAX_FILE_LEN 20

char buffer[TOTAL_CHARS];
int write_index = 0;
int read_index = 0;

pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_message(int thread_id, const char *role, const char *message)
{
    pthread_mutex_lock(&stdout_mutex);
    printf("Thread %d (%s): %s\n", thread_id, role, message);
    pthread_mutex_unlock(&stdout_mutex);
}

void *writer_thread(void *arg) {
    int thread_id = *(int *)arg;

    char symbol = '1' + thread_id;
    print_message(thread_id, "Writer", "started");

    for (size_t i = 0; i < CHARS_PER_THREAD; i++)
    {
        pthread_mutex_lock(&write_mutex);
        if (write_index < TOTAL_CHARS)
        {
            buffer[write_index++] = symbol;
        }
        pthread_mutex_unlock(&write_mutex);

        // Небольшая задержка для демонстрации параллельной работы
        usleep(100);
    }

    print_message(thread_id, "Writer", "finished");
    return NULL;
}

void *reader_thread(void *arg)
{
    int thread_id = *(int *)arg;

    char filename[MAX_FILE_LEN];
    sprintf(filename, "files/file%d.txt", thread_id + 1);

    print_message(thread_id, "Reader", "started");

    int file = open(filename, O_CREAT | O_RDWR);
    if (file < 0)
    {
        print_message(thread_id, "Reader", "failed to open file");
        return NULL;
    }

    for (size_t i = 0; i < CHARS_PER_THREAD; i++)
    {
        pthread_mutex_lock(&read_mutex);
        if (read_index < TOTAL_CHARS)
        {
            write(file, &buffer[read_index++], 1);
        }
        pthread_mutex_unlock(&read_mutex);

        // Небольшая задержка для демонстрации параллельной работы
        usleep(100);
    }

    close(file);

    print_message(thread_id, "Reader", "finished");
    return NULL;
}

int main() {
    pthread_t writers[NUM_THREADS];
    pthread_t readers[NUM_THREADS];

    int thread_ids[NUM_THREADS] = {0, 1, 2, 3};

    printf("\nMain: Starting writer threads...\n");
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&writers[i], NULL, writer_thread, &thread_ids[i]) != 0)
        {
            perror("Failed to create writer thread");
            return 1;
        }
    }

    printf("\nMain: Waiting for writer threads to complete...\n");
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    printf("\nMain: All writer threads completed. Starting reader threads...\n");
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&readers[i], NULL, reader_thread, &thread_ids[i]) != 0)
        {
            perror("Failed to create reader thread");
            return 1;
        }
    }

    printf("\nMain: Waiting for reader threads to complete...\n");
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    pthread_mutex_destroy(&write_mutex);
    pthread_mutex_destroy(&read_mutex);
    pthread_mutex_destroy(&stdout_mutex);

    printf("\nMain: Program completed successfully.\n");

    return 0;
}