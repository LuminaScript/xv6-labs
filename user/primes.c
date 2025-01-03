#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int SIZE = 34;
int fds[2]; // 0 - read, 1 - write

void child() {
    int buf[SIZE];
    memset(buf, 0, sizeof(buf));

    // process data from left neighbour
    int bytes = read(fds[0], buf, SIZE * (sizeof(int)));
    if (bytes == 0) {
        close(fds[0]); // close child pipe
        return ;
    }

    // drop data
    int min_num = buf[0], buf_out_idx = 0, n = bytes/sizeof(int);
    printf("prime %d\n", min_num);
    for (int i = 1; i < n; i++) {
        int num = buf[i];
        if(num % min_num) {
            buf[buf_out_idx++] = num;
        }
    }

    // feed data to right neighbour 
    if (buf_out_idx) {
        if (fork() == 0) { // child
            child(); // recursive
        } else {
            write(fds[1], buf, buf_out_idx * sizeof(int));
            wait((int *) 0);
        }
    } 
}

int main(int argc, char *argv[])
{
    int buf[SIZE], pid;
    for (int i = 2; i <= 35; i++) {
        buf[i - 2] = i;
    }

    pipe(fds);
    if ((pid = fork()) == 0) {
        child();
        close(fds[0]);
    } else {
        write(fds[1], buf, sizeof(buf));
        wait((int *) 0);
        close(fds[1]);
    }

    exit(0);
}