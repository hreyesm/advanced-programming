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

#define W 1024 // Window
#define H 30 // Max stars in histogram minus 1

typedef struct {
    int nFiles;
    int rangeStart;
    int rangeEnd;
    char *range;
    char *histogram;
} urn;

void printUrn(urn *u) {
    printf("%10d%10d%10d%40s\n", u->rangeStart, u->rangeEnd, u->nFiles, u->histogram);
}

void traverse(urn **urns, char *basePath, int *count, int *max) {
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
        // printf("\t%s\n", basePath);
        stat(basePath, &st);
        size = st.st_size;
        rangeStart = ceil(size / W) * W;
        rangeEnd = rangeStart + W - 1;
        u = *urns - 1;
        for (; u < *urns + *count; u++) {
            if (u->rangeEnd == 0) {
                u->range = (char *) malloc(sizeof(char) * 20);
                u->rangeStart = rangeStart;
                u->rangeEnd = rangeEnd;
                s1 = (char *) malloc(sizeof(char) * 10);
                s2 = (char *) malloc(sizeof(char) * 10);
                sprintf(s1, "%d-", u->rangeStart);
                sprintf(s2, "%d", u->rangeEnd);
                strcpy(u->range, strcat(s1, s2));
                printf("%s\n", u->range);
                u->nFiles = 1;
                break;
            } else if (u->rangeEnd == rangeEnd) {
                (u->nFiles)++;
                *max = fmax((double) *max, (double) u->nFiles);
                break;
            }
        }
        (*count)++;
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            traverse(urns, path, count, max);
        }
    }
    closedir(dir);
    free(path);
}

int main(int argc, char const *argv[]) {
    // Urns
    urn *urns = (urn *) malloc(sizeof(urn) * 1000);
    char *path = (char *) malloc(sizeof(char) * 1000);
    int count = 0;
    int max = 1;
    printf("\nHistogram\n---------\n\nPath: ");
    scanf("%s", path);
    traverse(&urns, path, &count, &max);

    // Histograms
    urn *u = urns - 1;
    int nStars = 0;
    char *c;
    for (; u->rangeEnd != 0; u++) {
        nStars = u->nFiles * H / max;
        u->histogram = (char *) malloc(sizeof(char) * nStars);
        strcpy(u->histogram, "*");
        c = u->histogram + 1;
        for (; c < u->histogram + nStars; c++) {
            strcpy(c, "*");
        }
    }
    
    // Print Urns
    printf("\n%10s%10s%10s%40s\n", "", "Urn (Size Range)", "File", "Histogram");
    printf("%10s%10s%10s%40s\n", "", "---", "----", "---------");
    u = urns - 1;
    int i = 0;
    for (; u->rangeEnd != 0; u++) {
        printUrn(u);
        i++;
    }
    printf("%10s%10s%10s%40s\n", "-----", "---", "----", "");
    printf("%10s%10d%10d%40s\n", "Total", i, count, "");

    free(urns);
    free(path);
    return 0;
}
