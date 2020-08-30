#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void gangScheduling() {
    printf("\nGang Scheduling\n---------------\n");
    int nProcesses, tCPU, nCPUs;
    printf("\nNumber of processes: ");
    scanf("%d", &nProcesses);
    printf("CPU time required for each process: ");
    scanf("%d", &tCPU);
    printf("Number of CPUs: ");
    scanf("%d", &nCPUs);
}

void tree(char *basePath, int root) {
    char *path = (char *) malloc(sizeof(char) * 1000);
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if (!dir) {
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            for (int i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0) {
                    printf("│");
                } else {
                    printf(" ");
                }
            }
            printf("├─%s\n", dp->d_name);
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            tree(path, root + 2);
        }
    }
    closedir(dir);
    free(path);
}

void histogram() {
    printf("\nHistogram\n---------\n");
    printf("\nPath: ");
    char *path = (char *) malloc(sizeof(char) * 1000);
    scanf("%s", path);
    printf("\nTree:\n\n");
    tree(path, 0);
    free(path);
}

int main(int argc, char const *argv[]) {
    printf("\nMenu\n----\n\n0. Exit program\n1. Gang scheduling\n2. Histogram\n\nChoose an option: ");
    int option;
    scanf("%d", &option);
    switch (option) {
        case 0:
            exit(0);
            break;
        case 1:
            gangScheduling();
            break;
        case 2:
            histogram();
            break;
        default:
            printf("Please enter a valid option number.");
            break;
        }
    return 0;
}
