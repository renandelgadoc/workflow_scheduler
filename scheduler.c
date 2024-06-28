#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 256

typedef struct process
{
    char *command;
    int pid;
    int *dependencies;
    struct process *next_on_wait;
} process;

typedef struct scheduler
{
    FILE *fptr;
    int process_pointer;
    process *on_wait;
    int *process_status;
} scheduler;

scheduler *create_scheduler(char *filepath)
{
    scheduler *scheduler_instance = (scheduler *)malloc(sizeof(scheduler));
    process *on_wait = NULL;

    if (!(scheduler_instance->fptr = fopen(filepath, "r")))
    {
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }

    scheduler_instance->on_wait = NULL;

    scheduler_instance->process_status = (int *)malloc(MAX_PROCESSES * sizeof(int));

    memset(scheduler_instance->process_status, 0, MAX_PROCESSES);

    return scheduler_instance;
}

process *create_process(scheduler *scheduler_instance)
{

    process *process_instance = (process *)malloc(sizeof(process));

    process_instance->command = malloc(8 * sizeof(char));

    process_instance->next_on_wait = NULL;

    char dependencies[255];

    int read_status = fscanf(scheduler_instance->fptr, "%d %s %s", &process_instance->pid, process_instance->command, dependencies);

    if (read_status == EOF && scheduler_instance->on_wait == NULL)
    {
        // Acabou
        free(process_instance->dependencies);
        free(process_instance->next_on_wait);
        free(process_instance);
        free(process_instance->command);
        fclose(scheduler_instance->fptr);
        free(scheduler_instance->on_wait);
        free(scheduler_instance->process_status);
        free(scheduler_instance);
        exit(0);
    }
    else if (read_status == EOF)
    {
        return NULL;
    }

    process_instance->dependencies = (int *)malloc((MAX_PROCESSES - 1) * sizeof(int));
    memset(process_instance->dependencies, 0, MAX_PROCESSES - 1);

    int i = 0;
    char c = dependencies[i];
    if (c == '0')
        return process_instance;

    int j = 0;

    while (c != '#')
    {
        if (c == ',')
        {
            c = dependencies[++i];
            continue;
        }
        process_instance->dependencies[j] = dependencies[i] - 48;
        c = dependencies[++i];
        j++;
    }
    return process_instance;
}

int check_dependecies(scheduler *scheduler_instance, process *current_process)
{
    for (int i = 0; current_process->dependencies[i] != 0; i++)
    {
        int j = current_process->dependencies[i] - 1;
        if (scheduler_instance->process_status[j] == 0)
        {
            return 1;
        }
    }
    return 0;
}

void add_to_wait_queue(scheduler *scheduler_instance, process *process_instance)
{
    if (scheduler_instance->on_wait == NULL)
    {
        scheduler_instance->on_wait = process_instance;
        return;
    }

    process *current = scheduler_instance->on_wait;

    while (current->next_on_wait != NULL)
    {
        current = current->next_on_wait;
    }

    current->next_on_wait = process_instance;
}

process *check_process_on_wait(scheduler *scheduler_instance)
{

    process *current = scheduler_instance->on_wait;
    process *ready_process;

    if (check_dependecies(scheduler_instance, current) == 0)
    {
        ready_process = current;
        scheduler_instance->on_wait = current->next_on_wait;
        return ready_process;
    }

    while (current->next_on_wait != NULL)
    {
        if (check_dependecies(scheduler_instance, current->next_on_wait) == 0)
        {
            ready_process = current->next_on_wait;
            current->next_on_wait = ready_process->next_on_wait;
            return ready_process;
        }
        current = current->next_on_wait;
    }

    return current->next_on_wait;
}

// Fazer o fork e chamar processo
void run_process(process *process_instance)
{
    printf("%s %d\n", "pid -", process_instance->pid);
    printf("%s %s\n", "command -", process_instance->command);
    printf("\n");

    free(process_instance->dependencies);
    // free(process_instance->next_on_wait);
    free(process_instance->command);
    free(process_instance);
}

void run_scheduler()
{
    scheduler *scheduler_instance = create_scheduler("./input");
    while (2 > 1)
    {
        process *process_instance;

        if (scheduler_instance->on_wait != NULL)
        {
            process_instance = check_process_on_wait(scheduler_instance);
            if (process_instance != NULL)
            {
                int j = process_instance->pid - 1;
                scheduler_instance->process_status[j] = 1;
                run_process(process_instance);
            }
        }

        process_instance = create_process(scheduler_instance);

        if (process_instance != NULL)
        {
            if (check_dependecies(scheduler_instance, process_instance) == 0)
            {
                int j = process_instance->pid - 1;
                scheduler_instance->process_status[j] = 1;
                run_process(process_instance);
            }
            else
            {
                add_to_wait_queue(scheduler_instance, process_instance);
            }
        }
    }
}
