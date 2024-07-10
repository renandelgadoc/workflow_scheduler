#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "scheduler.h"

int main(int argc, char *argv[]) {

    // for(int i = 0; i< argc; i++){
    //     printf("%s\n", argv[i]);
    // }

    mensagem mensagem_snd;
    mensagem_snd.pid = getpid();
    strcpy(mensagem_snd.msg, argv[1]);
    // printf("%s\n", mensagem_snd.msg);

    printf("%s %d\n", "child pid -", getpid());
    printf("Sleeping for 30 seconds...\n");
    sleep(30);

    if (msgsnd(atoi(argv[2]), &mensagem_snd, sizeof(mensagem_snd), 0) < 0)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("%s %s %s %s\n\n", "terminou - programa", mensagem_snd.msg, "/ processo", mensagem_snd.pid);

    exit(0);
}
