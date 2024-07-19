#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MAX_PROGRAMS 255

int double_check = 0;

typedef struct program
{
    char *command;
    char program_number[3];
    // pid_t child_pid;
    int *dependencies;
} program;

typedef struct scheduler
{
    int program_pointer;
    int cores;
    int total_cores;
    int *program_status;
    // int wait_count;
    program **program_queue;
    int qid;
} scheduler;

typedef struct mensagem
{
    long pid;
    char msg[30];
} mensagem;

scheduler *create_scheduler(char *cores)
{
    scheduler *scheduler_instance = (scheduler *)malloc(sizeof(scheduler));

    scheduler_instance->program_queue = (program **)malloc(MAX_PROGRAMS * sizeof(program *));
    memset(scheduler_instance->program_queue, 0, MAX_PROGRAMS);

    scheduler_instance->program_status = (int *)malloc(MAX_PROGRAMS * sizeof(int));
    memset(scheduler_instance->program_status, 0, MAX_PROGRAMS);

    // scheduler_instance->wait_count = 0;

    scheduler_instance->cores = atoi(cores);
    scheduler_instance->total_cores = atoi(cores);

    int idfila;

    if ((idfila = msgget(0x1223, IPC_CREAT | 0x1ff)) < 0)
    {
        printf("erro na criacao da fila\n");
        exit(1);
    }

    scheduler_instance->qid = idfila;

    return scheduler_instance;
}

void create_program_queue(scheduler *scheduler_instance, char *filepath)
{

    FILE *fptr;

    if (!(fptr = fopen(filepath, "r")))
    {
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }

    int i = 0;

    char *command = malloc(8 * sizeof(char));
    char dependencies[255];
    char pid[3];

    while (fscanf(fptr, "%s %s %s", pid, command, dependencies) != EOF)
    {

        scheduler_instance->program_queue[i] = (program *)malloc(sizeof(program));

        scheduler_instance->program_queue[i]->command = malloc(8 * sizeof(char));
        strcpy(scheduler_instance->program_queue[i]->command, command);

        strcpy(scheduler_instance->program_queue[i]->program_number, pid);

        scheduler_instance->program_queue[i]->dependencies = (int *)malloc((MAX_PROGRAMS - 1) * sizeof(int));
        memset(scheduler_instance->program_queue[i]->dependencies, 0, MAX_PROGRAMS - 1);

        int j = 0;
        char c = dependencies[j];
        if (c == '0')
        {
            i++;
            scheduler_instance->program_queue[i] = (program *)malloc(sizeof(program));
            scheduler_instance->program_queue[i]->command = malloc(8 * sizeof(char));
            continue;
        }

        int k = 0;

        while (c != '#')
        {
            if (c == ',')
            {
                c = dependencies[++j];
                continue;
            }
            scheduler_instance->program_queue[i]->dependencies[k] = dependencies[j] - 48;
            c = dependencies[++j];
            k++;
        }
        i++;
        memset(command, '\0', 8);
        memset(dependencies, '\0', MAX_PROGRAMS - 1);
    }
}

int check_dependecies(scheduler *scheduler_instance, program *current_program)
{
    for (int i = 0; current_program->dependencies[i] != 0; i++)
    {
        int j = current_program->dependencies[i] - 1;
        if (scheduler_instance->program_status[j] == 0)
        {
            return 1;
        }
    }
    return 0;
}

program *check_wait_queue(scheduler *scheduler_instance)
{

    int i = 0;

    while (scheduler_instance->program_queue[i] != NULL)
    {
        if (check_dependecies(scheduler_instance, scheduler_instance->program_queue[i]) == 0)
        {
            program *next = scheduler_instance->program_queue[i];
            while (scheduler_instance->program_queue[i] != NULL)
            {
                scheduler_instance->program_queue[i] = scheduler_instance->program_queue[i + 1];
                i++;
            }
            return next;
        }
        i++;
    }

    return NULL;
}

void run_program(int qid, program *program_instance)
{
    pid_t pid;

    printf("program - %s\n", program_instance->program_number);

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        char command[10] = "./";
        strcat(command, program_instance->command);
        char qid_string[15];
        sprintf(qid_string, "%d", qid);
        execl(command, program_instance->command, program_instance->program_number, qid_string, (char *)NULL);
    }

    free(program_instance->dependencies);
    free(program_instance->command);
    free(program_instance);
}

void run_scheduler(char *filepath, char *cores)
{

    scheduler *scheduler_instance = create_scheduler(cores);
    create_program_queue(scheduler_instance, filepath);
    program *program_instance;

    int i = 0;
    int rcv_flg;
    int child_status;

    mensagem mensagem_rec;

    while (scheduler_instance->program_queue[0] != 0 || scheduler_instance->cores < scheduler_instance->total_cores)
    {
        program_instance = check_wait_queue(scheduler_instance);
        if (program_instance != NULL)
        {
            scheduler_instance->cores--;
            run_program(scheduler_instance->qid, program_instance);
        }

        rcv_flg = IPC_NOWAIT;
        mensagem mensagem_rec;

        if (scheduler_instance->cores == 0 || double_check)
            rcv_flg = 0;

        if (msgrcv(scheduler_instance->qid, &mensagem_rec, sizeof(mensagem_rec), 0, rcv_flg) != -1)
        {
            printf("Killing program=%s process=%d\n", mensagem_rec.msg, mensagem_rec.pid);
            while (waitpid(mensagem_rec.pid, &child_status, 0) < 0)
                ;
            scheduler_instance->program_status[atoi(mensagem_rec.msg) - 1] = 1;
            int debug = scheduler_instance->program_status[atoi(mensagem_rec.msg) - 1];
            scheduler_instance->cores++;
            double_check = 0;
        }
        else{
            double_check = 1;
        }
    }

    struct msqid_ds buf;
    if (msgctl(scheduler_instance->qid, IPC_RMID, &buf) == -1)
    {
        perror("msgctl");
        exit(1);
    }

    free(scheduler_instance->program_status);
    free(scheduler_instance->program_queue);
    free(scheduler_instance);
    exit(0);
}
