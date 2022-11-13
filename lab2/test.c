#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <pthread.h>

void *thread_func(void *message) {
printf("%s\n", (const char *)message);
return message;
}
int main(void) {
pthread_t thread1, thread2;
const char *message1 = "Thread 1";
const char *message2 = "Thread 2";
// Create two threads, each with a different message.
pthread_create(&thread1, NULL, thread_func, (void *)message1);
pthread_create(&thread2, NULL, thread_func, (void *)message2);
// Wait for the threads to exit.
pthread_join(thread1, NULL);
pthread_join(thread2, NULL);
return 0;
}
