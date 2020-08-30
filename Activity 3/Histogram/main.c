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

void traverse(char *basePath, int root) {
    char *path = (char *) malloc(sizeof(char) * 100);
    struct dirent *dp;
    struct stat st;
    DIR *dir = opendir(basePath);
    if (!dir) {
        stat(basePath, &st);
        printf("File: %s\nSize: %lld bytes\n\n", basePath, st.st_size);
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            // Print Tree   
            // for (int i = 0; i < root; i++) {
            //     if (i % 2 == 0 || i == 0) {
            //         printf("│");
            //     } else {
            //         printf("  ");
            //     }
            // }
            // printf("├──%s\n", dp->d_name);
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            traverse(path, root + 2);
        }
    }
    closedir(dir);
    free(path);
}

int main(int argc, char const *argv[]) {
    printf("\nHistogram\n---------\n");
    printf("\nPath: ");
    char *path = (char *) malloc(sizeof(char) * 100);
    scanf("%s", path);
    printf("\nTree:\n\n");
    traverse(path, 0);
    free(path);
    return 0;
}
