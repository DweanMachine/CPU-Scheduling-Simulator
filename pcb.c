#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 1024

typedef struct PCB {
    //Process Control Block structure to represent a process in the system
    char* pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;          
    char state[12];   //"New", "Ready", "Running", or "Terminated"    
    struct PCB* next; //Points to the next process in the list
    int last_updated;
} PCB;

PCB* create_process(char* pid, int arrival_time, int burst_time, int priority) { 
    //Create a new process and initialize its attributes
    PCB* new_process = (PCB*)malloc(sizeof(PCB));
    
    //Allocate memory for the PID and copy the string
    new_process->pid = (char*)malloc(strlen(pid) + 1);
    strcpy(new_process->pid, pid);

    //Initialize the process attributes
    new_process->arrival_time = arrival_time;
    new_process->burst_time = burst_time;
    new_process->remaining_time = burst_time; 
    new_process->priority = priority;
    
    //Set the initial state of the process to "New"
    strcpy(new_process->state, "New");
    new_process -> next = NULL;
    return new_process;
}

void print_process(PCB* process) { //Print the details of a process
    printf("PID: %s, Arr: %d, Burst: %d, Rem: %d, Prio: %d, State: %s\n", process->pid, process->arrival_time, process->burst_time, process->remaining_time,  process->priority, process->state);
}

void neat_process_output(PCB* processes[]) {
    printf("NEW:\n");
    for (int i = 0; i < 4; i++) {
        if (strcmp(processes[i]->state, "New") == 0) {
            print_process(processes[i]);
        }
    }
    printf("RUNNING:\n");
    for (int i = 0; i < 4; i++) {
        if (strcmp(processes[i]->state, "Running") == 0) {
            print_process(processes[i]);
        }
    }
    printf("TERMINATED:\n");
    for (int i = 0; i < 4; i++) {
        if (strcmp(processes[i]->state, "Terminated") == 0) {
            print_process(processes[i]);
        }
    }
}

void free_processes(PCB* processes[]) {
    for (int i = 0; i < 4; i++) {
        free(processes[i]->pid);
        free(processes[i]);
        processes[i] = NULL;
    }
}

void update_process_state(PCB* process, char* new_state, int current_time) {
    strcpy(process->state, new_state);
    process->last_updated = current_time;
}

void increment_time(PCB* process, int current_time) {
    if (strcmp(process->state, "Running") != 0) return;  // only tick Running processes
    
    process->remaining_time--;

    if (process->remaining_time == 0) {
        update_process_state(process, "Terminated", current_time);
    }
}

//Initializes each process & state
void start_timer(PCB* processes[], int num_processes, int total_time) {
    for (int time = 0; time < total_time; time++) {
        printf("\nTime: %d\n", time);

        // Step 1 — Admit ONLY processes that arrive at this exact time
        for (int i = 0; i < num_processes; i++) {
            PCB* p = processes[i];
            if (p->arrival_time == time && strcmp(p->state, "New") == 0) {
                update_process_state(p, "Ready", time);   // NEW → READY
            }
        }

        // Step 2 — Assign CPU to first Ready process (FCFS)
        for (int i = 0; i < num_processes; i++) {
            PCB* p = processes[i];
            if (strcmp(p->state, "Ready") == 0) {
                update_process_state(p, "Running", time); // READY → RUNNING
                break;  // only one runs at a time
            }
        }

        // Step 3 — Tick and print all non-New, non-Terminated processes
        for (int i = 0; i < num_processes; i++) {
            PCB* p = processes[i];
            if (strcmp(p->state, "New") != 0 &&
                strcmp(p->state, "Terminated") != 0) {
                increment_time(p, time);
            }
        }
        neat_process_output(processes);
    }
}

int main() {
    PCB* processes[4];
    FILE* file = fopen("inputEx.txt", "r");
    if (file == NULL) {
        perror("Unable to open file");
        return EXIT_FAILURE;
    } 
    char pid[12];
    int arrival_time, burst_time, priority;
    int index = 0;

    char line_buffer[MAX_LINE_LENGTH];
    while (fgets(line_buffer, MAX_LINE_LENGTH, file) != NULL) {
        if (sscanf(line_buffer, "%s %d %d %d", pid, &arrival_time, &burst_time, &priority) == 4) {
            processes[index++] = create_process(pid, arrival_time, burst_time, priority);
        }
    }
    PCB* running = NULL; 
    fclose(file);

    
    printf("Initial Process States:\n");
    for (int i = 0; i < sizeof(processes) / sizeof(processes[0]); i++) {
        print_process(processes[i]);
    }
    printf("\n--- Simulating Process Execution: ---\n");

    // For simplicity, simulate all processes together
    start_timer(processes, sizeof(processes) / sizeof(processes[0]), 5);
    free_processes(processes);
    return 0;
}