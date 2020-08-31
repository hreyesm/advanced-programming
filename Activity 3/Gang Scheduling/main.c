/*
    Equipo: 5
    Miembros: Daniela Vignau (A01021698), Héctor Reyes (A01339607)
    Fecha: 3 de septiembre 2020
    Materia: Programación Avanzada
    Profesor: Vicente Cubells
    Actividad 3: Administración de un SO
    
    Instrucciones: Realice un programa en C donde simule el comportamiento del algoritmo de planificación por pandilla.
    El programa debe permitir al usuario entrar por teclado el número total de procesos a planificar,
    el tiempo de CPU que se requiere para cada uno, la relación que existe entre los procesos (grupos) y el número de CPUs.
    Los grupos de procesos deben ir planificándose según el criterio que Ud. defina (puede ser Round Robin, FIFO, etc.),
    garantizando siempre un máximo aprovechamiento de todas las CPUs.

    Nota: Para simular que un proceso consume tiempo de CPU, puede utilizar la función sleep()
*/  

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int gid;
    int cpu_time;
} proc;

typedef struct {
    int gid;
    proc * p_list;
} gang;

void print_processes(proc *, int);
void print_gangs(gang *, int);
int compare (const void *, const void *);

int main(int argc, char const *argv[]) {
    int n_proc; 
    int cont = 0;
    int cpu;
    printf("\n-------------------------------------\n");
    printf("            Gang Scheduling");
    printf("\n-------------------------------------\n");
    
    printf("\nNumber of processes to create: ");
    scanf("%d", &n_proc);
    
    proc * list_proc = (proc *) malloc(sizeof(proc) * n_proc);
    proc * p_start = list_proc;
    proc * p_end = list_proc + n_proc;
    
    for(; p_start < p_end; ++p_start) {
        printf("\nProcess ID: %d\n", cont);
        p_start->pid = cont;
        
        printf("\tGang ID: ");
        scanf("%d", &p_start->gid);
        
        printf("\tCPU time: ");
        scanf("%d", &p_start->cpu_time);
        cont++;
    }
    print_processes(list_proc, n_proc);
    qsort(list_proc, cont, sizeof(proc), compare);
    print_processes(list_proc, n_proc);

    int n_gangs = 1;
    proc * a = list_proc;
    proc * b;
    proc * c = list_proc;

    for(; a < p_end; ++a) {
        for(b = c + 1; b < p_end; ++b) {
            if(a->gid != b->gid) {
                n_gangs++;
            }
            break;
        }
    }
    
    printf("\nTotal number of gangs: %d", n_gangs);
    
    gang * list_gangs = (gang *)malloc(sizeof(gang) * n_gangs);
    gang * g_start = list_gangs;
    gang * g_end = list_gangs + n_gangs;
    
    proc * d = list_proc;
    proc * e = list_proc + n_proc;
    
    for(; g_start < g_end; ++g_start) {
        for(; d < e; ++d) {
            g_start->gid = d->gid;
            if(g_start->gid == d->gid){
                g_start->p_list = d;
                printf("\nProcess %d added to GID %d", d->pid, g_start->gid);
            }
        }
    }
    
    print_gangs(list_gangs, n_gangs);
    
    
    // printf("\nNumber of CPUs: ");
    // scanf("%d", &cpu);
    
    free(list_gangs);
    free(list_proc);
    printf("\nMemory freed\n");
    return 0;
}

int compare (const void * a, const void * b) {
   return ( ((proc*)a)->gid - ((proc*)b)->gid );
}

void print_processes(proc * list_proc, int n) {
    proc * aux = list_proc;
    proc * final = list_proc + n;
    printf("\n-------------------------------------\n");
    printf("           List of processes");
    printf("\n-------------------------------------\n");
    
    printf("\nGID: Gang ID\tCPUT: CPU time\n\n");
    printf("PID\tGID\tCPU\n\n");
    for(; aux < final; ++aux) 
        printf("%d\t %d\t %d\n", aux->pid, aux->gid, aux->cpu_time);
}

void print_gangs(gang * list_gang, int n) {
    gang * aux = list_gang;
    gang * final = list_gang + n;
    printf("\n-------------------------------------\n");
    printf("           List of gangs");
    printf("\n-------------------------------------\n");
    
    printf("\nGID: Gang ID\tCPUT: CPU time\n\n");
    printf("GID\tPID\n\n");
    for(; aux < final; ++aux) 
        printf("%d\t%d\n", aux->gid, aux->p_list->pid);
}