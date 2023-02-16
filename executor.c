#include "executor.h"
#include "cmdutils.h"

void executeCmd(char* cmd){
    char** cmdMap = splitCmd(cmd);
    if(validateCmd(cmdMap)) {
        runCmd(cmdMap);
    } else {
        fprintf(stderr, "Error: invalid command\n");
    }
    free(cmdMap);
    return;
}

int runCmd(char** cmdMap){
    if(cmdMap[0] == NULL) return 1;
    if(strcmp(cmdMap[0], "cd") == 0 || strcmp(cmdMap[0], "exit") == 0 
        || strcmp(cmdMap[0], "fg") == 0 || strcmp(cmdMap[0], "jobs") == 0) handleBuiltInCmd(cmdMap);
    return 0;
}

int handleBuiltInCmd(char** cmdMap) {
    if(strcmp(cmdMap[0], "exit") == 0) {
        exit(0);
    }
    // if(strcmp(cmdMap[0], "jobs") == 0) {
    //     jobs();
    // }
    // if(strcmp(cmdMap[0], "fg")) {
    //     fg(cmdMap[1]);
    // }
    if(strcmp(cmdMap[0], "cd") == 0) {
        chdir(cmdMap[1]);
    }
    return 0;
}