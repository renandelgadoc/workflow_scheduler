// #include <stdio.h>

#include "scheduler.h"

int main(int argc, char *argv[])
{

    // printf("PID = %jd\n", (intmax_t) getpid());

    if (argc != 2)
        return 1;

    run_scheduler(argv[1]);
}
