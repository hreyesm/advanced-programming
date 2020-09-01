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
#define H 30 // Max stars in histogram - 1

typedef struct {
    off_t size;
    int rangeStart;
    int rangeEnd;
} entry;

typedef struct {
    int nFiles;
    int rangeStart;
    int rangeEnd;
    char *histogram;
} urn;

void printEntry(entry *e) {
    printf("\n\tSize: %lld bytes", e->size);
    printf("\nRange: %d-%d bytes\n", e->rangeStart, e->rangeEnd);
}

void printUrn(urn *u) {
    printf("%10d%10d%10d%40s", u->rangeStart, u->rangeEnd, u->nFiles, u->histogram);
    printf("\n");
}

void traverse(urn **urns, entry **entries, char *basePath, int *count, int *max) {
    char *path = (char *) malloc(sizeof(char) * 1000);
    struct dirent *dp;
    struct stat st;
    DIR *dir = opendir(basePath);
    if (!dir) {
        printf("\t%s\n", basePath);
        stat(basePath, &st);
        (*entries + *count)->size = st.st_size;
        (*entries + *count)->rangeStart = ceil((*entries + *count)->size / W) * W;
        (*entries + *count)->rangeEnd = (*entries + *count)->rangeStart + W - 1;
        urn *u = *urns - 1;
        for (; u < *urns + *count; u++) {
            if (u->rangeEnd == 0) {
                u->rangeStart = (*entries + *count)->rangeStart;
                u->rangeEnd = (*entries + *count)->rangeEnd;
                u->nFiles = 1;
                break;
            } else if (u->rangeEnd == (*entries + *count)->rangeEnd) {
                (u->nFiles)++;
                *max = fmax((double) *max, (double) u->nFiles);
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
            traverse(urns, entries, path, count, max);
        }
    }
    closedir(dir);
    free(path);
}

int main(int argc, char const *argv[]) {
    // Urns
    entry *entries = (entry *) malloc(sizeof(entry) * 1000000);
    urn *urns = (urn *) malloc(sizeof(urn) * 1000);
    char *path = (char *) malloc(sizeof(char) * 1000);
    int count = 0;
    int max = 1;
    printf("\nHistogram\n---------\n\nPath: ");
    scanf("%s", path);
    traverse(&urns, &entries, path, &count, &max);

    // Histograms
    urn *u = urns - 1;
    int nStars = 0;
    for (; u->rangeEnd != 0; u++) {
        nStars = u->nFiles * H / max;
        u->histogram = (char *) malloc(sizeof(char) * nStars);
        strcpy(u->histogram, "*");
        char *c = u->histogram + 1;
        for (; c < u->histogram + nStars; c++) {
            strcpy(c, "*");
        }
    }
    
    // Print Urns
    printf("\n%10s%10s%10s%40s\n", "", "Range", "Files", "Histogram");
    printf("%10s%10s%10s%40s\n", "", "-----", "-----", "---------");
    u = urns - 1;
    for (; u->rangeEnd != 0; u++) {
        printUrn(u);
    }
    printf("\nTotal: %d\n", count);

    free(entries);
    free(urns);
    free(path);
    return 0;
}
