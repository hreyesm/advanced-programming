/*  Actividad 3: Histograma
    Curso: Programación avanzada
    Profesor: Vicente Cubells Nonell
    Autores:
        Daniela Vignau León (A01021698)
        Héctor Alexis Reyes Manrique (A01339607)
    Fecha: 3 de septiembre de 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct {
    char *file;
    off_t size;
} entry;

void printEntry(entry *e) {
    printf("\nFile: %s", e->file);
    printf("\nSize: %lld bytes\n", e->size);
}

void traverse(entry **entries, entry **e, char *basePath, int size, int *count) {
    char *path = (char *) malloc(sizeof(char) * 100);
    struct dirent *dp;
    struct stat st;
    DIR *dir = opendir(basePath);
    if (!dir) {
        (*entries + *count)->file = (char *) malloc(sizeof(char) * 100);
        strcpy((*entries + *count)->file, basePath);
        stat(basePath, &st);
        (*entries + *count)->size = st.st_size;
        (*count)++;
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            traverse(entries, e, path, size, count);
        }
    }
    closedir(dir);
    free(path);
}

int main(int argc, char const *argv[]) {
    int size = sizeof(entry);
    int count = 0;
    entry *entries = (entry *) malloc(size * 100);
    entry *e = entries;
    char *path = (char *) malloc(sizeof(char) * 100);
    printf("\nHistogram\n---------\n\nPath: ");
    scanf("%s", path);
    traverse(&entries, &e, path, size, &count);
    entry *temp = entries;
    entry *last = entries + count;
    for (; temp < last; temp++) {
        printEntry(temp);
    }
    free(path);
    return 0;
}
