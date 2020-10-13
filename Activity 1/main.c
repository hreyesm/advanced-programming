/*  Actividad individual 1: Aritmética de apuntadores
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autor: Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 20-8-2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 5

typedef struct {
    char *first;
    char *last;
    int age;
    char *phone;
    int bed;
} patient;

void printPatient(patient *p) {
    printf("\nBed %d\tFirst Name: %s\n\tLast Name: %s\n\tAge: %d\n\tPhone: %s\n", p->bed, p->first, p->last, p->age, p->phone);
}

void discharge(patient *p) {
    p->first = NULL;
    p->last = NULL;
    p->age = 0;
    p->phone = NULL;
    free(p->first);
    free(p->last);
    free(p->phone);
}

void exitProgram(patient *p, patient *end, patient *beds) {
    for (p = beds; p < end; p++) {
        if (p->first != NULL) {
            discharge(p);
        }
    }
    free(beds);
    exit(0);
}

void addNewPatient(int assigned, int *occupied, patient *p, patient *end, patient *beds) {
    printf("\n1. Add new patient\n––––––––––––––––––––––––––––––––––––––––\nNew patients are assigned the leftmost available bed by default.\n");
    assigned = 0;
    for (p = beds; p < end; p++) {
        if (p->first == NULL) {
            break;
        }
        (assigned)++;
    }
    p = beds + assigned;
    p->first = (char *) malloc(sizeof(char) * 25);
    p->last = (char *) malloc(sizeof(char) * 25);
    p->phone = (char *) malloc(sizeof(char) * 25);
    printf("\nFirst name: ");
    scanf("%s", p->first);
    printf("Last name: ");
    scanf("%s", p->last);
    printf("Age: ");
    scanf("%d", &p->age);
    printf("Phone: ");
    scanf("%s", p->phone);
    p->bed = assigned;
    *(beds + assigned) = *p;
    printf("\nThe following patient has been added:\n");
    printPatient(p);
    (*occupied)++;
}

void viewAllPatients(patient *p, patient *end, patient *beds) {
    printf("\n2. View all patients\n––––––––––––––––––––––––––––––––––––––––\n");
    for (p = beds; p < end; p++) {
        if (p->first != NULL) {
            printPatient(p);
        }
    }
}

void viewPatientByBedNumber(int input, int total, patient *p, patient *beds) {
    printf("\n3. View patient by bed number\n––––––––––––––––––––––––––––––––––––––––");
    printf("\nBed number: ");
    scanf("%d", &input);
    p = beds + input;
    if (input < 0 || input >= total) {
        printf("\nPlease enter a valid bed number [0-%d].\n", total - 1); 
    } else if (p->first == NULL) {
        printf("\nBed %d is available.\n", input);
    } else {
        printPatient(p);
    }
}

void dischargePatientByBedNumber(int input, int total, int *available, int *occupied, patient *p, patient *beds) {
    printf("\n4. Discharge patient by bed number\n––––––––––––––––––––––––––––––––––––––––");
    printf("\nBed number: ");
    scanf("%d", &input);
    p = beds + input;
    if (input < 0 || input >= total) {
        printf("\nPlease enter a valid bed number [0-%d].\n", total - 1);
    } else if (p->first == NULL) {
        printf("\nBed %d is available.\n", input);
    } else {
        printf("\n%s %s has been discharged.\n", p->first, p->last);
        discharge(p);
        (*available)++;
        (*occupied)--;
        printf("Bed %d is now available.\n", input);
    }
}

void menu(int input, int assigned, int *total, int *available, int *occupied, patient *p, patient *beds, patient *end) {
    printf("\nMenu\n––––––––––––––––––––––––––––––––––––––––\nOccupied beds: %d\nAvailable beds: %d\n(Total: %d)\n\n0. Exit program\n1. Add new patient\n2. View all patients\n3. View patient by bed number\n4. Discharge patient by bed number\n\nChoose an option: ", *occupied, *available, *total);
    scanf("%d", &input);
    switch (input) {
    case 0:
        exitProgram(p, end, beds);
        break;
    case 1:
        addNewPatient(assigned, occupied, p, end, beds);
        break; 
    case 2:
        viewAllPatients(p, end, beds);
        break;
    case 3:
        viewPatientByBedNumber(input, *total, p, beds);
        break;
    case 4:
        dischargePatientByBedNumber(input, *total, available, occupied, p, beds);
        break;
    default:
        printf("\nPlease enter a valid option number.\n");
        break;
    }
}

int main(int argc, char const *argv[]) {
    int available, assigned, occupied = 0, total = N;
    int input;
    patient *beds = (patient *) malloc(sizeof(patient) * N);
    patient *p, *end;
    while (1) {
        available = total - occupied;
        if (available == 0) {
            total += N;
            available = total - occupied;
            beds = (patient *) realloc(beds, sizeof(patient) * total);
            printf("\n%d new beds have been allocated.\n", N);
        }
        end = beds + total;
        menu(input, assigned, &total, &available, &occupied, p, beds, end);
    }
    return 0;
}
