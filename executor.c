#include "executor.h"
#include "cmdutils.h"

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
    int argc = 0, idx = -1;
    int appendMode = 0;
    char* writeTo = NULL, *readFrom = NULL;
    while(cmdMap[++idx] != NULL){
        if(strcmp("<", cmdMap[idx]) == 0) readFrom = cmdMap[++idx];
        else if(strcmp(">", cmdMap[idx]) == 0) writeTo = cmdMap[++idx];
        else if(strcmp(">>", cmdMap[idx]) == 0) {
            writeTo = cmdMap[++idx];
            appendMode = 1;
        }
        else if(strcmp("|", cmdMap[idx]) == 0) break;
        else argc++;
    }
    char** argv = malloc((argc+1)*sizeof(char*));
    argv[argc] = NULL;
    cmdMap[0] = getSanitizedCmd(cmdMap[0]);
    // printf("cmdMap %s, %d\n", cmdMap[0], argc);
    while(--argc >= 0) argv[argc] = cmdMap[argc];
    return sysCall(argv, readFrom, writeTo, appendMode);
}

int sysCall(char** argv, char* readFrom, char* writeTo, int appendMode){
    int pid = fork();
    if(pid == 0) {
        handleIORedirect(readFrom, writeTo, appendMode);
        execv(argv[0], argv);
        exit(5);
    }
    else {
        int exitCode = 0;
        waitpid(pid, &exitCode, 0);
        free(argv);
        if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 4) return -4;
        if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 5) return -5;
    }

    return 0;
}

void handleIORedirect(char* readFrom, char* writeTo, int appendMode) {
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

int handleBuiltInCmd(char** cmdMap) {
    if(strcmp(cmdMap[0], "cd") == 0) {
        return chdir(cmdMap[1]);
    }
    if(strcmp(cmdMap[0], "exit") == 0) {
        exit(0);
    }
    // if(strcmp(cmdMap[0], "jobs") == 0) {
    //     jobs();
    // }
    // if(strcmp(cmdMap[0], "fg")) {
    //     fg(cmdMap[1]);
    // }

    return 0;
}