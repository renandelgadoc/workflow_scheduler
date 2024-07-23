#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "scheduler.h"
#include <time.h>


int block = 1;

void leave_block(int signum)
{
    // printf("Caught signal %d, coming out...\n", signum);
    block = 0;
}

int main(int argc, char *argv[])
{
    clock_t start_time = clock();
    if (start_time == (clock_t)-1) {
        perror("clock");
        return 1;
    }

    signal(SIGALRM, leave_block);

    // printf("%s %d\n", "child pid -", getpid());
    // printf("Busy Waiting for 30 seconds...\n");
    // sleep(15);
    alarm(30);
    while (block)
        ;

    clock_t end_time = clock();
    if (end_time == (clock_t)-1) {
        perror("clock");
        return 1;
    }

    char buffer[100];
    snprintf(buffer, sizeof(buffer), " %.6f", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    mensagem mensagem_snd;
    mensagem_snd.pid = getpid();
    strcpy(mensagem_snd.msg, argv[1]);
    strcat(mensagem_snd.msg, buffer);

    if (msgsnd(atoi(argv[2]), &mensagem_snd, sizeof(mensagem_snd), 0) < 0)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    // printf("%s %s %s %s\n\n", "terminou - programa", mensagem_snd.msg, "/ processo", mensagem_snd.pid);

    exit(0);
}
