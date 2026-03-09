#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int num = 20;
    char *string = "Some text...";

    printf("hello world (pid: %d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid: %d)\n", (int) getpid());
        char *myargs[3];
        myargs[0] = strdup("ls");   // program: "ls" (list files)
        myargs[1] = strdup("-lah"); // flags: detailed (l), all files (a), human-readable (h)
        myargs[2] = NULL;           // marks end of array
        execvp(myargs[0], myargs);  // runs listing
    } else {
        // parent goes down this path (original process)
        int wc = wait(NULL);
        printf("hello, I am parent of %d (wc: %d) (pid: %d)\n", rc, wc, (int) getpid());
    }

    printf("num: %d ; string: %s\n", num, string);
    return 0;
}


