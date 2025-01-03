#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void error_exit(char *error_msg) {
    fprintf(2, error_msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    if (argc != 2) {
        error_exit("[Usage]: sleep <sleep_time_in_seconds>\nExample: sleep 10");
    }

    int sec = atoi(argv[1]);
    sleep(sec);
    exit(0);
}
