/*  Actividad 3: Histograma
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autores:
        Daniela Vignau León (A01021698)
        Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 3 de septiembre de 2020
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define H 30 // Max characters in histogram

typedef struct {
    int files;
    int rangeEnd;
    char *range;
    char *histogram;
} urn;

void printUrn(urn *u) {
    printf("%20s%10d%35s\n", u->range, u->files, u->histogram);
}

void traverse(urn **urns, char *basePath, int window, int *count, int *max) {
    char *path = (char *) malloc(sizeof(char) * 1000);
    struct dirent *dp;
    struct stat st;
    off_t size;
    int rangeStart;
    int rangeEnd;
    char *s1;
    char *s2;
    urn *u;
    DIR *dir = opendir(basePath);
    if (!dir) {
        stat(basePath, &st);
        size = st.st_size;
        // printf("\t%s (%lld bytes)\n", basePath, size);
        rangeStart = ceil(size / window) * window;
        rangeEnd = rangeStart + window - 1;
        u = *urns - 1;
        for (; u < *urns + *count; u++) {
            if (u->range == NULL) {
                u->files = 1;
                u->rangeEnd = rangeEnd;
                u->range = (char *) malloc(sizeof(char) * 40);
                s1 = (char *) malloc(sizeof(char) * 20);
                s2 = (char *) malloc(sizeof(char) * 20);
                sprintf(s1, "%d-", rangeStart);
                sprintf(s2, "%d", rangeEnd);
                strcpy(u->range, strcat(s1, s2));
                free(s1);
                free(s2);
                break;
            } else if (u->rangeEnd == rangeEnd) {
                (u->files)++;
                *max = fmax((double) *max, (double) u->files);
                break;
            }
        }
        (*count)++;
        free(path);
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            traverse(urns, path, window, count, max);
        }
    }
    closedir(dir);
    free(path);
}

int main(int argc, char const *argv[]) {
    urn *urns = (urn *) malloc(sizeof(urn) * 1000);
    char *path = (char *) malloc(sizeof(char) * 1000);
    int window;
    int count = 0, max = 1;
    printf("\nHistogram\n---------\n\nPath: ");
    scanf("%s", path);
    printf("Window in bytes (e.g. 10000): ");
    scanf("%d", &window);
    if (window > 0) {
        traverse(&urns, path, window, &count, &max);

        // Generate histograms
        urn *u = urns - 1;
        int chars = 0;
        char *c;
        for (; u->rangeEnd != 0; u++) {
            chars = u->files * H / max;
            u->histogram = (char *) malloc(sizeof(char) * chars);
            strcpy(u->histogram, "#");
            c = u->histogram + 1;
            for (; c < u->histogram + chars; c++) {
                strcpy(c, "#");
            }
        }
        
        // Print table
        printf("\n%20s%10s%35s\n", "Urn (Bytes)", "Files", "Histogram");
        printf("%20s%10s%35s\n", "-----------", "-----", "---------");
        u = urns - 1;
        int i = 0;
        for (; u->rangeEnd != 0; u++) {
            printUrn(u);
            i++;
        }
        printf("%20s%10s\n", "-----------", "-----");
        printf("%20d%10d\n", i, count);

        // Free individual urns
        u = urns - 1;
        for (; u->rangeEnd != 0; u++) {
            u->files = 0;
            u->rangeEnd = 0;
            free(u->range);
            free(u->histogram);
        }
    }
    free(urns);
    free(path);
    return 0;
}
