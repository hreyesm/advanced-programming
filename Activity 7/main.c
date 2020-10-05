#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int record = 1;

void handler(int signal) {
    record = 0;
    printf("Timer finished\n");
}

void manageDir(char *dirPath, DIR *dir, struct dirent *nextFile) {
    char filePath[256];
    if (dir) {
        while ((nextFile = readdir(dir)) != NULL) {
            sprintf(filePath, "%s/%s", dirPath, nextFile->d_name);
            remove(filePath);
        }
    } else  {
        mkdir(dirPath, 0777);
    }
}

void printDir(char *dirPath, DIR *dir, struct dirent *nextFile) {
    char filePath[256];
    struct stat st;
    off_t size;
    printf("\n%10s%20s\n", "File", "Size (Bytes)");
    printf("%10s%20s\n", "----", "------------");
    if (dir) {
        while ((nextFile = readdir(dir)) != NULL) {
            if (strcmp(nextFile->d_name, ".") != 0 && strcmp(nextFile->d_name, "..") != 0) {
                sprintf(filePath, "%s/%s", dirPath, nextFile->d_name);
                stat(filePath, &st);
                size = st.st_size;
                printf("%10s%20lld\n", nextFile->d_name, size);
            }
        }
    }
}
 
int main(int argc, char *argv[]) {
    sigset_t signals;
    sigfillset(&signals);
    sigdelset(&signals, SIGALRM);
    sigdelset(&signals, SIGINT);
    sigprocmask(SIG_BLOCK, &signals, NULL);
    
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGALRM, &sa, 0);
      
    int argument, index, n, t;
    char *input = NULL;
    char *dirPath = "./datos";
    DIR *dir = opendir(dirPath);
    struct dirent *nextFile;
    
    while ((argument = getopt (argc, argv, "n:t:")) != -1) {
        switch(argument) {
            case 'n':
                input = optarg;
                if (isdigit(*input)) {
                    n = atoi(input);
                }
                break;
            case 't':
                input = optarg;
                if (isdigit(*input)) {
                    t = atoi(input);
                }
                break;            
            default:
                abort();
        } 
    }
    
    manageDir(dirPath, dir, nextFile);

    char newFile[10];
    for (int i = 0; i < n; ++i) {
        FILE *fp;
        sprintf(newFile, "%s/a%d", dirPath, i);
        fp = fopen(newFile, "w");
        alarm(t);
        
        while (record == 1) {
            fprintf(fp, "x");
        }

        fclose(fp);
        memset(newFile, 0, 10);
        record = 1;
    }
    
    dir = opendir(dirPath);
    printDir(dirPath, dir, nextFile);
    
    return 0;
}