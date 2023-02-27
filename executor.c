#include "executor.h"
#include "cmdutils.h"
#include <stdlib.h>

#define MIN(X,Y) ((X > Y) ? Y : X)
 
char* jobsTable[800] = {NULL};

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
    int argc = 0, idx = -1, argd = 1000, readFd = -1;
    int pipefd[2] = {-1,-1};

    while(cmdMap[++idx] != NULL){
        if(strcmp("|", cmdMap[idx]) == 0) {
            readFd = pipefd[0];
            if(pipefd[1] != -1) {
                close(pipefd[1]);
                pipefd[0] = -1;
            }
            pipe(pipefd);
            int status = extractCmdAndRun(cmdMap, argc, idx, argd, readFd, pipefd[1]);
            argc = 0; argd = 1000; idx++;
            if(status < 0) return status;
        }
        if(strcmp("<", cmdMap[idx]) == 0 || strcmp(">>", cmdMap[idx]) == 0 || strcmp(">", cmdMap[idx]) == 0) argd = MIN(argd, idx);
        argc++;
    }
    readFd = pipefd[0];
    if(pipefd[1] != -1) {
        close(pipefd[1]);
        pipefd[1] = -1;
    } 
    return extractCmdAndRun(cmdMap, argc, idx, argd, readFd, -1);
}

int extractCmdAndRun(char** cmdMap, int argc, int idx, int argd, int readFd, int writeFd){ 
    int start = idx - argc;
    argd = MIN(argd - start, argc);
    char** argv = malloc((argc+1)*sizeof(char*));
    char** argvd = malloc((argd+1)*sizeof(char*));
    argv[argc] = NULL;
    argvd[argd] = NULL;
    cmdMap[start] = getSanitizedCmd(cmdMap[start]);
    while(--argc >= 0) argv[argc] = cmdMap[start+argc];
    while(--argd >= 0) argvd[argd] = cmdMap[start+argd];
    return sysCall(argv, argvd, readFd, writeFd);
}

int sysCall(char** argv, char** argvd, int read, int write){
    int pid = fork();
    if(pid == 0) {
        signal(SIGTSTP, suspendHandler);
        handleIORedirect(argv, read, write);
        execvp(argvd[0], argvd);
        exit(5);
    }
    else {
        if(read != -1) close(read);
        int exitCode = 0;
        waitpid(pid, &exitCode, WUNTRACED);
        if(WIFSTOPPED(exitCode)) putInMap(jobsTable, pid, argv);
        free(argv);
        free(argvd);
        if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 4) return -4;
        else if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 5)  return -5;
    }

    return 0;
}

void handleIORedirect(char** argv, int read, int write) {
    int appendMode = 0, idx = -1;
    char* writeTo = NULL, *readFrom = NULL;
    char* program = argv[0];
    char* newCmd = malloc(strlen(program)+9);
    strcpy(newCmd, "/usr/bin/");
    strcat(newCmd, program);
    int progAccess = access(program, R_OK);
    int cmdAccess = access(newCmd, R_OK);
    free(newCmd);
    if(progAccess == -1 && cmdAccess == -1) exit(5);
    
    if(read != -1){
        dup2(read, 0);
        close(read);
    }

    if(write != -1){
        dup2(write, 1);
        close(write);
    }

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
    if(sig){};
    raise(SIGSTOP);
}

int handleBuiltInCmd(char** cmdMap) {
    if(strcmp(cmdMap[0], "cd") == 0) {
        return chdir(cmdMap[1]);
    }
    if(strcmp(cmdMap[0], "exit") == 0) {
        if(jobsTable[0] != NULL) fprintf(stderr, "Error: there are suspended jobs\n");
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
    int idx = 0;
    while(jobsTable[idx] != NULL) {
        int j = 0;
        while(jobsTable[idx][j++] != ' ');
        printf("[%d] %s\n", idx+1, jobsTable[idx] + j);
        idx++;
    }
}

void fg(char* jobNum) {
    int index = atoi(jobNum);
    int i = -1;
    while(jobsTable[++i] != NULL);
    if(index > i) fprintf(stderr,"Error: invalid job\n");
    else {
        char** argv = removeFromMap(jobsTable, --index);
        int pid = atoi(argv[0]);
        kill(pid, SIGCONT);
        int status = 0;
        waitpid(pid, &status, WUNTRACED);
        if(WIFSTOPPED(status)) putInMap(jobsTable, -1, argv);
        if(WIFEXITED(status)) return;
    }
}