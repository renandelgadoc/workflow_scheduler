#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROGRAMS 10

typedef struct program
{
    char *command;
    int pid;
    int *dependencies;
} program;

typedef struct scheduler
{
    int program_pointer;
    int *program_status;
    program **program_queue;
} scheduler;

scheduler *create_scheduler()
{
    scheduler *scheduler_instance = (scheduler *)malloc(sizeof(scheduler));

    scheduler_instance->program_queue = (program **)malloc(MAX_PROGRAMS * sizeof(program *));
    memset(scheduler_instance->program_queue, 0, MAX_PROGRAMS);

    scheduler_instance->program_status = (int *)malloc(MAX_PROGRAMS * sizeof(int));
    memset(scheduler_instance->program_status, 0, MAX_PROGRAMS);

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
    int pid;

    while (fscanf(fptr, "%d %s %s", &pid, command, dependencies) != EOF)
    {

        scheduler_instance->program_queue[i] = (program *)malloc(sizeof(program));
        
        scheduler_instance->program_queue[i]->command = malloc(8 * sizeof(char));
        strcpy(scheduler_instance->program_queue[i]->command, command);

        scheduler_instance->program_queue[i]->pid = pid;


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
        memset(dependencies, '\0', 255);
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

// Fazer o fork e chamar programo
void run_program(program *program_instance)
{
    pid_t pid;

    pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        printf("%s %d\n", "pid -", program_instance->pid);
        printf("%s %s\n", "command -", program_instance->command);
        printf("\n");
        exit(0);
    default:
        printf("Child is PID %jd\n", (intmax_t)pid);
        wait(NULL);
    }

    free(program_instance->dependencies);
    free(program_instance->command);
    free(program_instance);
}

void run_scheduler(char *filepath)
{
    scheduler *scheduler_instance = create_scheduler();
    create_program_queue(scheduler_instance, filepath);
    program *program_instance;

    int i = 0;

    while (scheduler_instance->program_queue[0] != 0)
    {
        program_instance = check_wait_queue(scheduler_instance);
        if (program_instance != NULL)
        {
            int j = program_instance->pid - 1;
            scheduler_instance->program_status[j] = 1;
            run_program(program_instance);
        }
    }

    free(scheduler_instance->program_status);
    free(scheduler_instance->program_queue);
    free(scheduler_instance);
    exit(0);
}
