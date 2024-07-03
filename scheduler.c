#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROGRAMS 255

typedef struct program
{
    char *command;
    int program_number;
    pid_t child_pid;
    int *dependencies;
} program;

typedef struct scheduler
{
    int program_pointer;
    int cores;
    int *program_status;
    int wait_count;
    program **program_queue;
} scheduler;

scheduler *create_scheduler(char *cores)
{
    scheduler *scheduler_instance = (scheduler *)malloc(sizeof(scheduler));

    scheduler_instance->program_queue = (program **)malloc(MAX_PROGRAMS * sizeof(program *));
    memset(scheduler_instance->program_queue, 0, MAX_PROGRAMS);

    scheduler_instance->program_status = (int *)malloc(MAX_PROGRAMS * sizeof(int));
    memset(scheduler_instance->program_status, 0, MAX_PROGRAMS);

    scheduler_instance->wait_count = 0;

    scheduler_instance->cores = atoi(cores);

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

        scheduler_instance->program_queue[i]->program_number = pid;

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

// Fazer o fork e chamar programo
void run_program(program *program_instance)
{
    pid_t pid;

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        printf("%s %d\n", "program number -", program_instance->program_number);
        program_instance->child_pid = pid;
        execl(program_instance->command, (char *)NULL);
        exit(0);
    }
    // default:
    // printf("Child is PID %jd\n", (intmax_t)pid);

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

    while (scheduler_instance->program_queue[0] != 0)
    {
        if (scheduler_instance->cores > 0)
        {

            program_instance = check_wait_queue(scheduler_instance);
            if (program_instance != NULL)
            {
                scheduler_instance->cores--;
                run_program(program_instance);
                scheduler_instance->wait_count++;
            }
        }

        if (scheduler_instance->wait_count > scheduler_instance->cores)
        {
            wait(NULL);
            scheduler_instance->wait_count--;
            scheduler_instance->cores++;
        }
    }

    for (; scheduler_instance->wait_count > 0; scheduler_instance->wait_count--)
    {
        wait(NULL);
        scheduler_instance->cores++;
    }

    free(scheduler_instance->program_status);
    free(scheduler_instance->program_queue);
    free(scheduler_instance);
    exit(0);
}
