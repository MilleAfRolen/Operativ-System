#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>



int main() {
    int MAX_SIZE = 100;
    int MAX_NUM_MSG = 10;
    struct mq_attr attr;
    // Form the queue attributes
    attr.mq_maxmsg = MAX_NUM_MSG;
    attr.mq_msgsize = MAX_SIZE;
    mode_t qmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    pid_t pid = fork();

    if (pid == 0) {
        //producer
        char *my_mq = "/mymq";
        char *write_msg = "hello";
        mqd_t mqd;
        // Open an existing message queue
        mqd = mq_open(my_mq, O_WRONLY | O_CREAT, qmode, &attr);
        if(mqd == -1) {
            perror("mqd error");
            return(0);
        }
        // Write "hello" to the message queue
        //printf("SENDING: %s\n", write_msg);
        mq_send(mqd, write_msg, strlen(write_msg), 0);
        
        // Close the message queue
        mq_close(mqd);
    } else if (pid > 0) {
        

        //consumer

        char *my_mq = "/mymq";
        char buf[MAX_SIZE];
        mqd_t mqd;
        // Create message queue
        //printf("bruh\n");
        mqd = mq_open(my_mq, O_RDONLY | O_CREAT, qmode, &attr);
        
        // Read the message from the message queue
        mq_receive(mqd, buf, MAX_NUM_MSG, NULL);
        printf("Message: %s\n", buf);
        // Close the message queue
        mq_close(mqd);
    }
    return 1;
}