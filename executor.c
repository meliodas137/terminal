#include "executor.h"
#include "cmdutils.h"
#include <stdlib.h>

#define MIN(X,Y) ((X > Y) ? Y : X)
 
char* jobsTable[800];
int totalJobs = 0;

void executeCmd(char* cmd){
    char** cmdMap = splitCmd(cmd);
    if(validateCmd(cmdMap)) {
        int code = runCmd(cmdMap);
        if(code == -1) fprintf(stderr, "Error: invalid directory\n"); //cd dir error
        if(code == -2) fprintf(stderr, "Error: there are suspended jobs\n"); //exit error
        if(code == -3) fprintf(stderr, "Error: invalid job\n"); //fg error
        if(code == -4) fprintf(stderr, "Error: invalid file\n"); //file read error
        if(code == -5) fprintf(stderr, "Error: invalid program\n"); //program invalid
    } else {
        fprintf(stderr, "Error: invalid command\n");
    }
    freeStrMap(cmdMap);
    return;
}

int runCmd(char** cmdMap){
    if(cmdMap[0] == NULL) return 1;
    if(strcmp(cmdMap[0], "cd") == 0 || strcmp(cmdMap[0], "exit") == 0 
        || strcmp(cmdMap[0], "fg") == 0 || strcmp(cmdMap[0], "jobs") == 0) return handleBuiltInCmd(cmdMap);
    
    return handlePrograms(cmdMap);
}

int handlePrograms(char** cmdMap) {
    int argc = 0, idx = -1, argd = 1000;

    while(cmdMap[++idx] != NULL){
        if(strcmp("|", cmdMap[idx]) == 0) break;
        if(strcmp("<", cmdMap[idx]) == 0 || strcmp(">>", cmdMap[idx]) == 0 || strcmp(">", cmdMap[idx]) == 0) argd = MIN(argd, idx);
        argc++;
    }
    argd = MIN(argd, argc);
    char** argv = malloc((argc+1)*sizeof(char*));
    char** argvd = malloc((argd+1)*sizeof(char*));
    argv[argc] = NULL;
    argvd[argd] = NULL;
    cmdMap[0] = getSanitizedCmd(cmdMap[0]);
    // printf("cmdMap %s, %d\n", cmdMap[0], argc);
    while(--argc >= 0) argv[argc] = cmdMap[argc];
    while(--argd >= 0) argvd[argd] = cmdMap[argd];
    return sysCall(argv, argvd);
}

int sysCall(char** argv, char** argvd){
    int pid = fork();
    if(pid == 0) {
        // printf("Waiting for the child process\n");
        signal(SIGTSTP, suspendHandler);
        signal(SIGSTOP, suspendHandler);
        handleIORedirect(argv);
        execv(argvd[0], argvd);
        exit(5);
    }
    else {
        putinJobsTable(pid, argv);
        signal(SIGUSR1, jobsHandler);
        signal(SIGCHLD, childHandler);
        pause();
        free(argv);
        free(argvd);
        // int exitCode = 0;
        // printf("Waiting for the process to sigchld\n");
        // waitpid(pid, &exitCode, 0);
        // printf("Waiting for the process AFTER sigchld\n");
        // free(argv);
        // if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 4) return -4;
        // else if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 5)  return -5;
    }

    return 0;
}

void putinJobsTable(int pid, char** argv){
    char* s_pid = malloc(1000);
    sprintf(s_pid, "%d", pid);
    int idx = 0;
    while(argv[idx] != NULL) {
        strcat(s_pid, " ");
        strcat(s_pid, argv[idx++]);
    }
    jobsTable[totalJobs++] = s_pid;
}

void handleIORedirect(char** argv) {
    int appendMode = 0, idx = -1;
    char* writeTo = NULL, *readFrom = NULL;

    while(argv[++idx] != NULL){
        if(strcmp("<", argv[idx]) == 0) readFrom = argv[++idx];
        else if(strcmp(">", argv[idx]) == 0) writeTo = argv[++idx];
        else if(strcmp(">>", argv[idx]) == 0) {
            writeTo = argv[++idx];
            appendMode = 1;
        } 
    }

    free(argv);

    if(readFrom != NULL) {
        int accessible = access(readFrom, R_OK);
        if(accessible == -1) exit(4);
        else {
            int fd = open(readFrom, O_RDONLY);
            dup2(fd, 0);
            close(fd);
        }
    }

    if(writeTo != NULL) {
        int fd ;
        if(appendMode == 1) fd = open(writeTo, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR);
        else fd = open(writeTo, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
        dup2(fd, 1);
        close(fd);
    }
    return;
}

void suspendHandler(int sig){
    kill(getppid(), SIGUSR1);
    signal(sig, SIG_DFL);
}

void jobsHandler(int sig) {
}

void childHandler(int sig) {
    int exitCode = 0;
    int pid = waitpid(-1, &exitCode, WNOHANG);
    // printf("totaljobs before childHandle %d\n", totalJobs);
    totalJobs = removeFromMap(jobsTable, pid, -1, totalJobs);
    if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 4) fprintf(stderr, "Error: invalid file\n");
    else if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 5)  fprintf(stderr, "Error: invalid program\n");
}

int handleBuiltInCmd(char** cmdMap) {
    if(strcmp(cmdMap[0], "cd") == 0) {
        return chdir(cmdMap[1]);
    }
    if(strcmp(cmdMap[0], "exit") == 0) {
        if(totalJobs != 0) fprintf(stderr, "Error: there are suspended jobs\n");
        else exit(0);
    }
    if(strcmp(cmdMap[0], "jobs") == 0) {
        jobs();
    }
    if(strcmp(cmdMap[0], "fg") == 0) {
        fg(cmdMap[1]);
    }

    return 0;
}

void jobs(){
    int idx = -1;
    while(++idx < totalJobs) {
        int j = 0;
        while(jobsTable[idx][j++] != ' ');
        printf("[%d] %s\n", idx+1, jobsTable[idx] + j);
    }
}

void fg(char* jobNum) {
    int index = atoi(jobNum);
    if(index > totalJobs) fprintf(stderr,"Error: invalid job");
    else {
        int pid = removeFromMap(jobsTable, -1, --index, totalJobs);
        kill(pid, SIGCONT);
        signal(SIGUSR1, jobsHandler);
        signal(SIGCHLD, childHandler);
        pause();
    }
}