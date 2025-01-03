#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int fds[2];
    pipe(fds);
    int pid = fork();

    if (pid == 0) {
        char buf[2];
        int child_pid = getpid();
        read(fds[1], buf, 1);
        fprintf(0, "%d: received ping\n", child_pid);
        write(fds[1], buf, 1);
    } else {
        char buf[2];
        int par_pid = getpid();
        write(fds[0], "x", 1);
        read(fds[0], buf, 1);
        fprintf(0, "%d: received pong\n", par_pid);
    }
    exit(0);
}