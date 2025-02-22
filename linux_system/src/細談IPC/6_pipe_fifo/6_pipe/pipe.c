#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int          pipefd[2];
    pid_t        cpid;
    char         buf[128];
    int          readlen;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return -1;
    }

    if (pipe(pipefd) < 0) {
        fprintf(stderr, "pipe: %s\n", strerror(errno));
        return -1;
    }

    cpid = fork();

    if (cpid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return -1;
    }

    if (0 == cpid) {    /* Child reads from pipe */
        close(pipefd[1]);          /* Close unused write end */

        readlen = read(pipefd[0], buf, 128);

        if (readlen < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            return -1;
        }

        write(STDOUT_FILENO, buf, readlen);

        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);

        return 0;
    } else {            /* Parent writes argv[1] to pipe */
        close(pipefd[0]);          /* Close unused read end */
        sleep(2);
        write(pipefd[1], argv[1], strlen(argv[1]));

        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */

        return 0;
    }
}
