#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <omp.h>

int buffer = 0;
pthread_t thread[3];
pthread_mutex_t lock;

void *thread_func(void *arg) {
    int count = 0;
    //allocate memory in order to return local variable
    int* result = malloc(sizeof(int));
    while (1) {
        pthread_mutex_lock(&lock);
        if (buffer >= 15) {
            pthread_mutex_unlock(&lock);
            *result = count;
            //return how many times thread worked on the buffer
            return (void*) result;
        }
        printf("TID: %u, PID: %d, Buffer: %d\n", pthread_self(), getpid(), buffer);
        ++buffer;
        ++count;
        pthread_mutex_unlock(&lock);
    }
}


int main(void) {
    int totalCounter = 0;
    int* count;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&thread[i], NULL, thread_func, NULL) != 0) {
            perror("Could not create thread %d\n");
            return 1;
        }
        //printf("Thread %u (%d) has started\n", thread[i], i);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(thread[i], (void **) &count);
        //printf("Thread %u (%d) has finished\n", thread[i], i);
        printf("TID %u worked on the buffer %d times\n", thread[i], *count);
        totalCounter += *count;
    }

    printf("Total buffer accesses: %d\n", totalCounter);
    
    pthread_mutex_destroy(&lock);

    

    return 0;
}