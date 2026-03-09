/*
Execution: gcc -o pcb pcb.c
Output: ./pcb
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

void neat_process_output(PCB* processes[]) { //Properly prints the output in a nice neat graph
    printf("NEW:\n");
    for (int i = 0; i < 4; i++) {
        if (strcmp(processes[i]->state, "New") == 0) {
            print_process(processes[i]);
        }
    }
    printf("READY:\n");
    for (int i = 0; i < 4; i++) {
        if (strcmp(processes[i]->state, "Ready") == 0) {
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
    //only increment remaining time for running processes
    if (strcmp(process->state, "Running") != 0) return;  
    
    process->remaining_time--;

    if (process->remaining_time == 0) {
        update_process_state(process, "Terminated", current_time);
    }
}

//Initializes each process & state
void start_timer(PCB* processes[], int proc_count, int total_time, int schedulingAlgorithm) {
    for (int time = 0; time < total_time; time++) {
        printf("\nTime: %d\n", time);
        
        for (int i = 0; i < proc_count; i++) {
            PCB* p = processes[i];
            if (p->arrival_time == time && strcmp(p->state, "New") == 0) {
                update_process_state(p, "Ready", time);
            }
        }

        switch (schedulingAlgorithm) {
            case 1:
                first_come_first_serve(processes, proc_count, time);
                break;
            case 2:
                shortest_time_remaining_first(processes, proc_count, time);
                break;
            case 3:
                //priority_scheduling(processes, proc_count, time);
            default:
                printf("WHAAAAAAATTTT???");
                break;
        }
        neat_process_output(processes);

        //Increment time by 1
        for (int i = 0; i < proc_count; i++) {
            PCB* p = processes[i];
            if (strcmp(p->state, "New") != 0 &&
                strcmp(p->state, "Terminated") != 0) {
                increment_time(p, time);
            }
        }  
    }
}

void first_come_first_serve(PCB* processes[], int proc_count, int time) {    
    //Boolean value which implies if cpu is free or not
    bool cpu_free = true; 
    for (int i = 0; i < proc_count; i++) {
        if (strcmp(processes[i]->state, "Running") == 0) {
            cpu_free = false;
            break;
        }
    }

    //Checks if any processes are currently running on the CPU
    if (cpu_free) {
        for (int i = 0; i < proc_count; i++) {
            PCB* p = processes[i];
            if (strcmp(p->state, "Ready") == 0) {
                update_process_state(p, "Running", time);
                break;
            }
        }
    }
}

void shortest_time_remaining_first(PCB* processes[], int proc_count, int time) {

    //Keeps track of the process with shortest remaining time
    PCB* shortest = NULL;
    for (int i = 0; i < proc_count; i++) {
        PCB* p = processes[i];
        if (strcmp(p->state, "Ready") == 0 || strcmp(p->state, "Running") == 0) {
            if (shortest == NULL || p->remaining_time < shortest->remaining_time) {
                shortest = p;
            }
        }
    }

    // Step 3 — Preempt current Running process if shorter one exists
    for (int i = 0; i < proc_count; i++) {
        PCB* p = processes[i];
        if (strcmp(p->state, "Running") == 0 && p != shortest) {
            update_process_state(p, "Ready", time);   // preempt — Running → Ready
        }
    }

    //Promote shortest time to Running
    if (shortest != NULL && strcmp(shortest->state, "Ready") == 0) {
        update_process_state(shortest, "Running", time);
    }
}

void priority_scheduling(PCB* processes[], int proc_count, int time);

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

    int schedulingAlgorithm;
    printf("Choose scheduling algorithm:\n");
    printf("[1] First Come First Serve\n");
    printf("[2] Shortest Time Remaining First\n");
    printf("[3] Priority Scheduling\n");
    printf("[4] Round Robin\n");
    scanf("%d", &schedulingAlgorithm);
    
    printf("Initial Process States:\n");
    for (int i = 0; i < sizeof(processes) / sizeof(processes[0]); i++) {
        print_process(processes[i]);
    }
    printf("\n--- Simulating Process Execution: ---\n");

    // For simplicity, simulate all processes together
    start_timer(processes, sizeof(processes) / sizeof(processes[0]), 12, schedulingAlgorithm);
    free_processes(processes);
    return 0;
}