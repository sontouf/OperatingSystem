// wait system call

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid: %d)\n", (int)getpid());
    int rc = fork();
    // rc에 저장 이후 
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("I am child (pid : %d)\n", (int)getpid());
    } else {
        int wc = wait(NULL);
        printf("I am parent of %d (wc : %d) (pid : %d)\n", rc, wc, (int)getpid());
    }
    return 0;
}