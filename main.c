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

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/ipc.h>
// #include <sys/msg.h>
// #include <errno.h>

// struct mensagem
// {
//     long pid;
//     char msg[30];
// };

// int main()
// {
//     int pid, idfila, fd[2], estado;

//     struct mensagem mensagem_env, mensagem_rec;
//     if ((idfila = msgget(0x1223, IPC_CREAT | 0x1ff)) < 0)
//     {
//         printf("erro na criacao da fila\n");
//         exit(1);
//     }
//     pid = fork();
//     if (pid == 0)
//     {
//         mensagem_env.pid = getpid();
//         strcpy(mensagem_env.msg, "teste de mensagem");
//         msgsnd(idfila, &mensagem_env, sizeof(mensagem_env), 0);
//         exit(0);
//     }
//     msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec), 0, 0);
//     printf("mensagem recebida = %d %s\n", mensagem_rec.pid, mensagem_rec.msg);
//     wait(&estado);
//     exit(0);
// }
