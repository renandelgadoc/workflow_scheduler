#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "scheduler.h"

int block = 1;

void leave_block(int signum)
{
    printf("Caught signal %d, coming out...\n", signum);
    block = 0;
}

int main(int argc, char *argv[])
{

    signal(SIGALRM, leave_block);

    printf("%s %d\n", "child pid -", getpid());
    printf("Busy Waiting for 30 seconds...\n");
    // sleep(15);
    alarm(30);
    while (block)
        ;

    mensagem mensagem_snd;
    mensagem_snd.pid = getpid();
    strcpy(mensagem_snd.msg, argv[1]);
    // printf("%s\n", mensagem_snd.msg);
    // block = 0;

    if (msgsnd(atoi(argv[2]), &mensagem_snd, sizeof(mensagem_snd), 0) < 0)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("%s %s %s %s\n\n", "terminou - programa", mensagem_snd.msg, "/ processo", mensagem_snd.pid);

    exit(0);
}
