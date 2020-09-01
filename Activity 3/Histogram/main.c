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

typedef struct {
    off_t size;
    int rangeStart;
    int rangeEnd;
} entry;

typedef struct {
    int nFiles;
    int rangeStart;
    int rangeEnd;
} urn;

void printEntry(entry *e) {
    printf("\n\tSize: %lld bytes", e->size);
    printf("\n\tRange: %d - %d bytes\n", e->rangeStart, e->rangeEnd);
}

void printUrn(urn *u) {
    printf("\n\tRange: %d - %d", u->rangeStart, u->rangeEnd);
    printf("\n\tNumber of files: %d", u->nFiles);
}

void traverse(urn **urns, entry **entries, char *basePath, int *count) {
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
            traverse(urns, entries, path, count);
        }
    }
    closedir(dir);
    free(path);
}

int main(int argc, char const *argv[]) {
    // Entry Generation
    entry *entries = (entry *) malloc(sizeof(entry) * 1000000);
    urn *urns = (urn *) malloc(sizeof(urn) * 1000);
    char *path = (char *) malloc(sizeof(char) * 1000);
    int count = 0;
    printf("\nHistogram\n---------\n\nPath: ");
    scanf("%s", path);

    printf("\nFile paths:\n\n");

    traverse(&urns, &entries, path, &count);

    printf("\nTotal files: %d\n", count);
    
    // Print Urns
    printf("\nRanges:\n");
    urn *u = urns - 1;
    for (; u->rangeEnd != 0; u++) {
        printUrn(u);
        printf("\n");
    }

    free(entries);
    free(urns);
    free(path);
    return 0;
}
