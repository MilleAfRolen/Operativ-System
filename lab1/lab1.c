#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#define BUFFER_SIZE 100
#define MAX_SIZE 100
#define MAX_NUM_MSG 10


char *args[] = {"/bin/ls", "/", NULL};
char *args2[] = {"/usr/bin/wc", "-l", NULL};
int fd[2];
char *path = "/bin/ls";
char *path2 = "/usr/bin/wc";

int part1() {
    int ret;
    if (pipe(fd) == -1) {
        printf("pipe error");
    }

    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork Error");
    }

    if (pid == 0) {
        //Child Process, running ls command

        //duplicate the pipe write end to stdout
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        
        //execute ls command in root directory
        ret = execv(path, args);
        if (ret == -1) {
            perror("execv");
            exit(1);
        }
    } else if(pid > 0) {
        //wait until child is done
        wait(NULL);
        //duplicate the pipe read end to stdin
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        //wc command will read from stdin 
        ret = execv(path2, args2);
        if (ret == -1) {
            perror("execv");
            exit(1);
        }
    }
}

int part2() {
    char *my_mq = "/queue";
    char ch;
    char msg_ptr[1000];
    FILE *ptr;
    mqd_t mqd;
    struct mq_attr attr;


    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork Error");
    }
   
    if (pid == 0) {
        //Child process 1 - Producer

        mqd = mq_open(my_mq, O_WRONLY);
        ptr = fopen("part2.txt", "r");
        if(ptr == NULL) {
            perror("Error opening file");
            return(-1);
        }

        while(fscanf(ptr, "%s", msg_ptr) == 1) {
            mq_send(mqd, msg_ptr, sizeof msg_ptr, 0);
        }
        fclose(ptr);
       

        mq_close(mqd);
        
    } else if (pid > 0) {
        char buf[MAX_SIZE];
        attr.mq_maxmsg = MAX_NUM_MSG;
        attr.mq_msgsize = MAX_SIZE;
        mqd = mq_open(my_mq, O_RDONLY | O_CREAT, &attr);

        wait(NULL);
        
        mq_receive(mqd, buf, MAX_SIZE, NULL);
        printf("Message: %s\n", buf);

        mq_close(mqd);

    }
    return 0;
}

int main(int argc, char *argv[]) {
    part2();


    return 0;
}