#include "executor.h"
#include "cmdutils.h"

void executeCmd(char* cmd){
    char** cmdMap = splitCmd(cmd);
    if(validateCmd(cmdMap)) {
        runCmd(cmdMap);
    } else {
        printf("Error: invalid command\n");
        return;
    }
}

int runCmd(char** cmdMap){
    if(cmdMap) return 1;
    return 0;
}
