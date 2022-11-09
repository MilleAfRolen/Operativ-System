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



int main(int argc, char *argv[]) {
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
    return 0;
}