/*
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
https://stackoverflow.com/questions/30220691/how-to-get-empty-input-or-only-enter-in-c
https://www.educative.io/answers/how-to-write-regular-expressions-in-c
Class Slides
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "cmdutils.h"

int main(){
    char* cmd = malloc(1000);
    char* cwd = malloc(1000);
    if(getcwd(cwd, 1000) == NULL) {
        printf("Error: Unable to find the cwd");
        return -1;
    }
    char* dirName = getDirName(cwd);
    printf("[nyush %s]$ ", dirName);
    fflush(stdout);
    while(fgets(cmd, 1000, stdin) != NULL){
        if(!validateCmd(cmd)){
            printf("Error: invalid command\n");
        } else {
            char **cmdMap = splitCmd(cmd);
            int idx = 0;
            while(cmdMap[idx++] != NULL) free(cmdMap[idx]);
            free(cmdMap);
        }
        printf("[nyush %s]$ ", dirName);
        fflush(stdout);
    }
    free(cmd);
    free(cwd);
    return 0;
}