#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

void *thread_func(void *message) {

    printf("%s\n", (const char *)message);
    return message;

}


int main(void) {
    int buffer = 0;

    pid_t pid;
    pthread_t thread1, thread2, thread3;

    pthread_create(&thread1, NULL, thread_func, (void *) buffer);
    pthread_create(&thread2, NULL, thread_func, (void *) buffer);
    pthread_create(&thread3, NULL, thread_func, (void *) buffer);

    printf("TID: %d, PID: %d, Buffer: %d", pthread_self(), getpid(), buffer++);

    return 0;
}