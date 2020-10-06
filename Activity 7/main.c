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

typedef struct {
    int code;
    char name[10];
} signalInfo;

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
    sigprocmask(SIG_BLOCK, &signals, NULL);

    struct sigaction sa, saOld;
    sa.sa_handler = handler;
    sigaction(SIGALRM, &sa, 0);
      
    int argument, n, t;
    char *dirPath = "./datos", *input = NULL;
    DIR *dir = opendir(dirPath);
    struct dirent *nextFile;

    signalInfo signalList[] = {
        {SIGHUP, "SIGHUP"},
        {SIGINT, "SIGINT"},
        {SIGQUIT, "SIGQUIT"},
        {SIGILL, "SSIGILL"},
        {SIGTRAP, "SIGTRAP"},
        {SIGABRT, "SIGABRT"},
        {SIGBUS, "SIGBUS"},
        {SIGFPE, "SIGFPE"},
        {SIGUSR1, "SIGUSR1"},
        {SIGSEGV, "SIGSEGV"},
        {SIGUSR2, "SIGUSR2"},
        {SIGPIPE, "SIGPIPE"},
        {SIGTERM, "SIGTERM"},
        {SIGSTKFLT, "SIGTKFLT"},
        {SIGCHLD, "SIGCHLD"},
        {SIGCONT, "SIGCONT"},
        {SIGTSTP, "SIGTSTP"},
        {SIGTTIN, "SIGTTIN"},
        {SIGTTOU, "SIGTTOU"},
        {SIGURG, "SIGURG"},
        {SIGXCPU, "SIGXCPU"},
        {SIGXFSZ, "SIGXFSZ"},
        {SIGVTALRM, "SIGVTALRM"},
        {SIGPROF, "SIGPROF"},
        {SIGWINCH, "SIGWINCH"},
        {SIGPOLL, "SIGPOLL"},
        {SIGPWR, "SIGPWR"},
        {SIGSYS, "SIGSYS"},
    };

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

    for (int i = 0; i < n; i++) {
        FILE *fp;
        sigset_t waitingSignals;
        sprintf(newFile, "%s/a%d", dirPath, i);
        fp = fopen(newFile, "w");
        alarm(t);

        while (record == 1) {
            fprintf(fp, "x");
        }

        sigpending(&waitingSignals);
        for (int j = 0; j < sizeof(signalList) / sizeof(signalList[0]); j++) {
            if (sigismember(&waitingSignals, signalList[j].code)) {
                fprintf(fp, signalList[j].name);
            } 
        }
        
        fclose(fp);
        memset(newFile, 0, 10);
        record = 1;
    }
    
    dir = opendir(dirPath);
    printDir(dirPath, dir, nextFile);

    sigaction(SIGALRM, 0, &saOld);
    sigprocmask(SIG_UNBLOCK, &signals, NULL);

    return 0;
}