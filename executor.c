#include "executor.h"
#include <sys/wait.h>
#include "cmdutils.h"

void executeCmd(char* cmd){
    char** cmdMap = splitCmd(cmd);
    if(validateCmd(cmdMap)) {
        int code = runCmd(cmdMap);
        if(code == -1) fprintf(stderr, "Error: invalid directory\n"); //cd dir error
        if(code == -2) fprintf(stderr, "Error: there are suspended jobs\n"); //exit error
        if(code == -3) fprintf(stderr, "Error: invalid job\n"); //fg error
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
    
    else {
        return handlePrograms(cmdMap);
    }
    return 0;
}

int handlePrograms(char** cmdMap) {
    int argc = 0, idx = -1;
    regex_t notAllowed;
    regcomp(&notAllowed, "[\76\74\174]", REG_EXTENDED);
    while(cmdMap[++idx] != NULL){
        if(regexec(&notAllowed, cmdMap[idx], 0, NULL, 0) == REG_NOMATCH) argc++;
        else break;
    }
    char** argv = malloc((argc+1)*sizeof(char*));
    argv[argc] = NULL;
    cmdMap[0] = getSanitizedCmd(cmdMap[0]);
    while(--argc >= 0) argv[argc] = cmdMap[argc];
    return sysCall(argv);
}

int sysCall(char** argv){
    if(fork() == 0) {
        execv(argv[0], argv);
        // printf("errno %d", errno);
        exit(4);
    }
    int exitCode = 0;
    wait(&exitCode);
    free(argv);
    // printf("errno-- %d", errno);
    // if(WIFEXITED(errno)) printf("errno %d", WEXITSTATUS(errno));
    if(WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 4) return -5;
    return 0;
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