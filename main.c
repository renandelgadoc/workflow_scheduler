#include <stdio.h>
#include <unistd.h>

#include "scheduler.h"

#define NUMBER_OF_ARGUMENTS 4

int main(int argc, char *argv[])
{
    char *cores = NULL;
    char *filepath = NULL;

    for (int i = 1; i < NUMBER_OF_ARGUMENTS;)
    {
        if (strcmp(argv[i], "-cores") == 0 && (i + 1) < argc)
        {
            cores = argv[i + 1];
            i+=2;
        }
        else
        {
            filepath = argv[i];
            i++;
        }
    }

    if (cores == NULL)
    {
        printf("cores not provided\n");
        exit(1);
    }

    if (access(filepath, F_OK) != 0) {
        printf("input file is not valid\n");
        exit(1);
    }

    run_scheduler(filepath, cores);
}
