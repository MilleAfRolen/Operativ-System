#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define QUEUE_NAME "/queue"
#define QUEUE_MAX_MSG 10
#define QUEUE_MSGSIZE 1024

int main() {
    mqd_t mqd;
    FILE *ptr;
    mode_t qmode = S_IWUSR | S_IRUSR;
    struct mq_attr mqAttr;
    struct timespec timeoutAttr; 
    
    char msg_ptr[QUEUE_MSGSIZE];
    
    // Unlink to ensure no old queues exist with same name
    int status = mq_unlink(QUEUE_NAME);
    if (status < 0) {
        perror("Error Unlinking queue");
    }
    
    // Form the queue and timeout attributes
    mqAttr.mq_maxmsg = QUEUE_MAX_MSG;
    mqAttr.mq_msgsize = QUEUE_MSGSIZE;
    timeoutAttr.tv_sec = 5;

    //Create the queue
    mqd = mq_open(QUEUE_NAME, O_RDWR | O_CREAT | O_NONBLOCK, qmode, &mqAttr);
        if(mqd == -1) {
            perror("Could not create queue");
            exit(0);
        }
        else {
            printf("Opened Queue\n");
        }
    pid_t pid = fork();
    if (pid == 0) {
        //Child process

        //Open an existing message queue
        mqd = mq_open(QUEUE_NAME, O_RDWR);
        if(mqd == -1) {
            perror("Could not open child queue");
            exit(0);
        }
        else {
            printf("Opened Queue\n");
        }

        //Opens stream for textfile
        // ptr = fopen("part2.txt", "r");
        ptr = fopen("some_file.txt", "r");
        if(ptr == NULL) {
            perror("Error opening file");
            exit(-1);
        } else {
            printf("Successfully opened file\n");
        }
        
        //Send All data in textfile
        while (fread(&msg_ptr, sizeof(char), QUEUE_MSGSIZE, ptr)) {
            
            //printf("%d\n", strlen(msg_ptr));
            if(mq_send(mqd, msg_ptr, strlen(msg_ptr) - 1, 0) == -1) {
                perror("Could not send message");
                exit(-1);
            } else {
                printf("Sent Message\n");
                memset(msg_ptr, 0, sizeof msg_ptr);
            }
        }

        //Close stream to textfile
        fclose(ptr);
        
        // Close the message queue
        mq_close(mqd);
    } else if (pid > 0) {
        //Parent

        char buf[QUEUE_MSGSIZE];

        //wait for send
        wait(NULL);
        // Create message queue
        mqd = mq_open(QUEUE_NAME, O_RDWR);
        if(mqd == -1) {
            perror("Could not open parent queue");
            exit(0);
        }
        else {
            printf("Opened Queue\n");
        }
        
        // Read the message from the message queue
        while(1) {
            status = mq_timedreceive(mqd, buf, QUEUE_MSGSIZE, NULL, &timeoutAttr);
            // printf("%d\n", status);
            if (status < 0) {
                //close message queue
                mq_close(mqd);
                perror("\nCould not receive message");
                exit(-2);
            } else {
                // printf("Received message\n");
                printf("%s\n", buf);
                memset(buf, 0, sizeof buf);
            }
        }

    }
    return 1;
}