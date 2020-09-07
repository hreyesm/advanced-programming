/*
    Equipo: 5
    Miembros: Daniela Vignau (A01021698), Héctor Reyes (A01339607)
    Fecha: 3 de septiembre 2020
    Materia: Programación Avanzada
    Profesor: Vicente Cubells
    Actividad 3: Administración de un SO - Ejercicio 1
*/  

#include <stdio.h>
#include <stdlib.h>
// Imports for sleep()
#include <Windows.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define QUANTUM 3

typedef struct {
    int pid;            // Set as a counter
    int gid;            // USER
    int at;             // arrival time --> USER
    int bt;             // burst time --> USER
    int wt;             // waiting time
    int ret;            // return time
    int res;            // response time
    int tat;            // turn around time
    int finished;
} proc;

typedef struct {
    int gid;           
    int n_processes;
    proc * list;
    int tpt;             // total processing time
} gang;

typedef struct node {
    proc * p;
    struct node * next;
} node_t;

node_t * head = NULL;
int active_proc;

void print_processes(proc *, int);
void print_list_proc(proc *, int);
void print_gangs(gang *, int);
void print_p_gangs(proc *, int, int);

int sort_gang_id(const void *, const void *);
int sort_arrival_time(const void *, const void *);
int count_gangs(proc *, int);
void create_gangs(proc *, int, int, int);
void process_init(proc *, int);
void free_memory(gang *, int, proc *, int);

void read_file();
void manual_input();

void fcfs(proc *, int);
void program_init(proc *, int);
void scheduling(gang *, int, proc *, int, int);
void reduce_bt(proc *, int);
void enqueue(proc *, int);
proc * dequeue();

int main(int argc, char const *argv[]) { 
    head = malloc(sizeof(node_t));
    head->next = NULL;
    char mr = 'r';
    
    while(mr != 'e' || mr != 'E') {
        printf("\n--------------------------------------------------------------------------\n");
        printf("                        Gang Scheduling with FCFS");
        printf("\n--------------------------------------------------------------------------\n");
        
        printf("\nDo wou want to manually enter the process list, or read from a file?\n\n\tM - Manual entry\n\tR - Read file\n\tE - Exit program\n");
        printf("\nSelect and option (M/R/E): ");
        scanf("%s", &mr);

        switch(mr) {
            case 'r': 
                read_file();
                break;
            case 'R': 
                read_file();
                break;                     
            case 'm': 
                manual_input();
                break;
            case 'M': 
                manual_input();
                break;                       
            case 'E':            
                printf("\n\n                             Exiting program");
                printf("\n--------------------------------------------------------------------------\n\n");
                exit(0);
                break;
            case 'e':            
                printf("\n\n                             Exiting program");
                printf("\n--------------------------------------------------------------------------\n\n");
                exit(0);
                break;                
            default: 
                printf("\nNon-valid option. Try again\n");
                break;
        }

    }
    return 0;
}

void manual_input() {
    printf("\n--------------------------------------------------------------------------\n");
    printf("                      Manual Input of Process List");
    printf("\n--------------------------------------------------------------------------\n");
    int n_proc;
    int pid = 1;        // for process id 
    printf("\nNumber of processes to create: ");
    scanf("%d", &n_proc);
    active_proc = n_proc;

    proc * list_proc = (proc *) malloc(sizeof(proc) * n_proc);
    proc * p_start = list_proc;
    proc * p_end = list_proc + n_proc;
    
    printf("Please enter the information below. All inputs are expected to real numbers\n");

    /* Add information for each of the processes */
    for(; p_start < p_end; ++p_start) {
        printf("\nProcess ID: %d\n", pid);
        p_start->pid = pid;
        pid++;

        printf("\tGang ID: ");
        scanf("%d", &p_start->gid);
        
        printf("\tArrival time: ");
        scanf("%d", &p_start->at);
        
        printf("\tCPU time (Burst time): ");
        scanf("%d", &p_start->bt);

        // enqueue(p_start);
    }

    program_init(list_proc, n_proc);
}

void read_file() {
    printf("\n--------------------------------------------------------------------------\n");
    printf("                      Read Process List from File");
    printf("\n--------------------------------------------------------------------------\n");

    printf("\nIs there a file named 'file.txt' with the following structure? Example:\n");
    printf("\n\t6\n\t0 1 3 5\n\t1 1 0 4\n\t2 1 1 1\n\t3 2 5 3\n\t4 1 3 7\n\t5 0 8 2\n");
    printf("\nWhere: \n\t- The first line is the number of processes to create\n\t- Following lines are the processes to work with.\n\t- Each line is composed of:\n\t\t- Gang ID\n\t\t-  Process ID (sequentially ordered)\n\t\t- Arrival Time\n\t\t- Burst Time\n");
    printf("\n*** Please note there is a single space between values. And all\n    values are real numbers\n");
    int n_proc;
    char answer;
    printf("\nAnswer Y/N: ");
    scanf("%s", &answer);
    if(answer == 'N' || answer == 'n') {
        printf("\nPlease create a file with said structure and name.\n");
        printf("Exiting program\n\n");
        exit(0);
    } else {        
        FILE *file = fopen("file.txt", "r");

        if(file == NULL) {
            printf("Error opening the file. It is either non existent or the\nfile is damaged\n");
            exit(0);
        }

        fscanf(file, " %d", &n_proc);
        active_proc = n_proc;
    
        proc * list_proc = (proc *) malloc(sizeof(proc) * n_proc);
        proc * p_start = list_proc;
        proc * p_end = list_proc + n_proc;
        int i = 0;
        
        while(i < n_proc) { 
            fscanf(file, "%d %d %d %d", &p_start->gid, &p_start->pid, &p_start->at, &p_start->bt);
            // enqueue(p_start);
            p_start++;
            i++;
        }
        fclose(file);
        printf("\nSuccessfuly read process list information from the file\n");

        program_init(list_proc, n_proc);
    }   
}

void program_init(proc * list_proc, int n_proc) {
    int n_cpu;
    printf("\nEnter the number of CPUs: ");
    scanf("%d", &n_cpu);  
    
    process_init(list_proc, n_proc);        
    
    int n_gangs = count_gangs(list_proc, n_proc);  
    create_gangs(list_proc, n_proc, n_gangs, n_cpu);
}

int count_gangs(proc * list_proc, int n_proc) {
    proc * p_start = list_proc;
    proc * p_end = list_proc + n_proc;
    /* Sort processes increasingly by gang ID*/
    qsort(list_proc, n_proc, sizeof(proc), sort_gang_id);

    /* Count number of gangs */
    int n_gangs = 1;
    proc * p_next;
    p_start = list_proc;
    
    for(; p_start < p_end; ++p_start) {
        for(p_next = p_start + 1; p_next < p_end; ++p_next) {
            if(p_start->gid != p_next->gid) {
                n_gangs++;
                break;
            }
            break;
        }
    }
    return n_gangs;
}

void create_gangs(proc * list_proc, int n_proc, int n_gangs, int n_cpu) {
    proc * p_start = list_proc;
    proc * p_end = list_proc + n_proc;

    qsort(list_proc, n_proc, sizeof(proc), sort_gang_id);
    
    /* Create structures of gangs to append the total number of processes under it */
    gang * list_gangs = (gang *)malloc(sizeof(gang) * n_gangs);
    gang * g_start = list_gangs;
    gang * g_end = list_gangs + n_gangs;

    int test = 0;
    int gang_id, prev;
    p_start = list_proc;
    g_start = list_gangs;
    gang_id = 0;
    int flag = 1;
    prev = p_start->gid;
    
    /* Add  gangID */
    for(; p_start < p_end; ++p_start) {
        gang_id = p_start->gid;
        if(prev != p_start->gid) {
            prev = p_start->gid;
            flag = 1;
        }
        if(flag == 1) {
            while(g_start < g_end) {
                g_start->gid = gang_id;
                flag = 0;
                ++g_start;
                break;
            }
        } 
    }
    
    /* Count processes with the same gang ID */
    p_start = list_proc;
    g_start = list_gangs;
    for(; p_start < p_end; ++p_start) {
        for(; g_start < g_end; ++g_start) {
            if(p_start->gid == g_start->gid){
                test++;
                g_start->n_processes = test;
                break;
            } else {
                test = 0;
            }
        }
    }

    /* Allocate process memory to gangs */
    g_start = list_gangs;
    p_start = list_proc;
    int size;
    for(; g_start < g_end; ++g_start) {
        size = g_start->n_processes;
        g_start->list = (proc *)malloc(sizeof(proc) * size);
    }

    /* Insert processes into corresponding gang */
    proc * aux = g_start->list;
    g_start = list_gangs;
    p_start = list_proc;
    int curr_id, n, control;
    for(; g_start < g_end; ++g_start) {
        g_start->tpt = 0;
        curr_id = g_start->gid;
        n = g_start->n_processes;
        control = 1;
        aux = g_start->list;
        while(p_start < p_start + n) {
            *aux = *p_start;
            g_start->tpt += aux->bt;
            aux++;
            if(control == n) {
                p_start++;
                break;
            } else {
                control++;
            }
            p_start++;
        }
    }

    scheduling(list_gangs, n_gangs, list_proc, n_proc, n_cpu);
}

void scheduling(gang * list_gangs, int n_gangs, proc * list_proc, int n_proc, int n_cpu) {
    print_gangs(list_gangs, n_gangs);
    printf("\n");
    int time = 0;
    int i = 0;

    // !!!!!
    if(n_cpu > n_proc) {
        n_cpu = n_proc;
    }
    printf("                                  CPU\n");
    for(; i < n_cpu; ++i) {
        // printf("%8d", i);
    }
    printf("\n--------------------------------------------------------------------------\n");
    
    proc * gang_in;
    proc * last_proc = list_proc + n_proc;
    proc * current_set;
    int n_scheduled;
    proc * p = list_proc;
    active_proc = n_proc;
    gang * g = list_gangs;

    qsort(list_proc, n_proc, sizeof(proc), sort_gang_id);
    while(active_proc > 0) {
        gang_in = list_proc;
        printf("Scheduling gang %d\n", g->gid);
        for(; gang_in < last_proc; gang_in+=n_cpu) {
            current_set = gang_in;
            n_scheduled = n_cpu;     
            // enqueue(current_set, n_cpu);
            fcfs(current_set, n_scheduled);
            reduce_bt(current_set, n_scheduled);
            sleep(QUANTUM);
            printf("\n");
            time += QUANTUM;
        }
        g++;
        // printf("Number of active processes: %d", active_proc);
    }

    printf("\n\nTotal processing time: %d", time);

    printf("\n\n");
    // print_list_proc(list_proc, n_proc);

    // print_gangs(list_gangs, n_gangs); 
    free_memory(list_gangs, n_gangs, list_proc, n_proc);
}

void reduce_bt(proc * list_proc, int n) {
    proc * aux = list_proc;
    proc * end = list_proc + n;

    for(; aux < end; ++aux) {
        if(aux->bt >= QUANTUM) {
            aux->bt -= QUANTUM;
        } else if(aux->bt < QUANTUM) {
            aux->bt = 0;
        }
        if(aux->bt == 0 && aux->finished == FALSE) {
            aux->finished = TRUE;
            active_proc--;
            printf("\t\tProcess %d is finished", aux->pid);
        }
    }
}

void enqueue(proc * list, int n) {
    node_t * current = head;
    while(current->next != NULL) {
        current = current->next;
    }
    proc * aux = list;
    proc * final = list + n;
    for(; aux < final; ++aux) {
        if(aux->finished == FALSE) {
            proc * process = (proc *) malloc(sizeof(proc));
            current->next = malloc(sizeof(node_t));
            current->next->p = process;
            *current->next->p = *aux;
            current->next->next = NULL;
            // printf("Process %d queued sucessfully\n", aux->pid);
        } else {
            aux->pid = -1; // to see which ones are finished
        }
    }
}

proc * dequeue() {
    proc * p;
    if(active_proc == 0){
        p = NULL;
    } else if(active_proc == 1) {
        p = head->p;
    } else {
        node_t * next_proc = head->next;
        p = head->p;
        free(head);
        if(next_proc != NULL) {
            head = next_proc;
        } else {
            head = NULL;
        }
    }
    active_proc--;
    printf("\t\nProcess %d dequeued sucessfully\n", p->pid);
    return p;
}

/* Helper functions */

void free_memory(gang * list_gangs, int n_gangs, proc * list_proc, int n_proc) {
    gang * g_start = list_gangs;
    gang * g_end = list_gangs + n_gangs;
    proc * p_start = g_start->list;    
    proc * p_end = g_start->list + g_start->n_processes;
    for(; g_start < g_end; ++g_start) {
        for(; p_start < p_end; ++p_start){
            free(p_start);
        }
    }
    free(list_gangs);
    free(list_proc);

    printf("Memory freed correctly\n");
}

void process_init(proc * list_proc, int n) {
    proc * aux = list_proc;
    proc * final = list_proc + n;

    for(; aux < final; ++aux) {
        aux->wt = 0;
        aux->ret = 0;
        aux->res = 0;
        aux->tat = 0;
        aux->finished = FALSE;
    }
}

void fcfs(proc * list_proc, int n) {
    /* Sort processes by arrival time */
    qsort(list_proc, n, sizeof(proc), sort_arrival_time);
    int totalResT = 0;              // response time
    int totalWT = 0;                // waiting time
    int totalTAT = 0;               // turnaround time
    int totalRetT = 0;              // return time
    
    proc * first = list_proc;
    proc * aux = list_proc + 1;
    proc * final = list_proc + n;
    proc * list = list_proc;

    first->wt = 0;
    first->ret = first->bt;
    first->tat = first->ret - first->at;
    first->res = 0;

    totalResT += first->res;
    totalWT += first->wt;
    totalTAT += first->tat;
    totalRetT += first->bt;    

    for(; aux < final; aux++) {        
        aux->wt = totalRetT - aux->at;
        aux->ret = totalRetT + aux->bt;
        aux->tat = aux->ret - aux->at;
        aux->res = aux->wt;

        totalResT += aux->res;
        totalWT += aux->wt;
        totalTAT += aux->tat;
        totalRetT += aux->bt;
    }
    
    print_processes(list_proc, n);

    double avgWT = (double)totalWT/(double)n;
    double avgTAT = (double)totalTAT/(double)n;
    double avgResT = (double)totalResT/(double)n;

    // printf("\nAverage Waiting Time    = %.2lf\n", avgWT);
	// printf("Average Turnaround Time = %.2lf\n", avgTAT);
	// printf("Average Response Time   = %.2lf\n\n", avgResT);
}

void print_processes(proc * list_proc, int n) {
    proc * aux = list_proc;
    proc * final = list_proc + n;
    for(; aux < final; ++aux) 
        printf("%8d", aux->pid);
}

int sort_gang_id(const void * a, const void * b) {
   return (((proc *)a)->gid - ((proc *)b)->gid);
}

int sort_arrival_time(const void * a, const void * b) {
   return (((proc *)a)->at - ((proc *)b)->at);
}

void print_list_proc(proc * list_proc, int n) {
    proc * aux = list_proc;
    proc * final = list_proc + n;
    
    printf("\nGID: Gang ID\t\tRetT: Return Time\nPID: Process ID\t\tResT: Response Time\nBT: Burst time\t\tWT: Waiting Time\nAT: Arrival Time\tTAT: Turnaround Time\n\n");
    printf("GID\tPID\tBT\tAT\tRetT\tResT\tWT\tTAT\n\n");
    for(; aux < final; ++aux) 
        printf("%d\t %d\t %d\t %d \t%d \t%d \t%d \t %d\n", aux->gid, aux->pid, aux->bt, aux->at, aux->ret, aux->res, aux->wt, aux->tat);
}

void print_p_gangs(proc * list_proc, int n, int gid) {
    proc * aux = list_proc;
    proc * final = list_proc + n;
    for(; aux < final; ++aux) 
        printf("%d\t %d\t %d\t %d \t%d \t%d \t%d \t %d\n", gid, aux->pid, aux->bt, aux->at, aux->ret, aux->res, aux->wt, aux->tat);
}

void print_gangs(gang * list_gang, int n) {
    gang * aux = list_gang;
    gang * final = list_gang + n;
    printf("\n--------------------------------------------------------------------------\n");
    printf("                             List of processes");
    printf("\n--------------------------------------------------------------------------\n");
    
    printf("\nGID: Gang ID\t\tRetT: Return Time\nPID: Process ID\t\tResT: Response Time\nBT: Burst time\t\tWT: Waiting TIme\nAT: Arrival Time\tTAT: Turnaround Time\n\n");
    printf("GID\tPID\tBT\tAT\tRetT\tResT\tWT\tTAT\n\n");
    for(; aux < final; aux++) {
        print_p_gangs(aux->list, aux->n_processes, aux->gid);
    }
    printf("\n--------------------------------------------------------------------------\n");
    printf("                            Information by gang");
    printf("\n--------------------------------------------------------------------------\n");
    
    printf("\nGID: Gang ID\nnPROC: Number of processes\nTPT: Total processing time\n\n");
    printf("GID\tnPROC\tTPT\n\n");
    aux = list_gang;
    for(; aux < final; aux++) {
        printf("%d \t%d \t%d\n", aux->gid, aux->n_processes, aux->tpt );
    }

    printf("\nTotal number of gangs: %d\n\n", n);
}