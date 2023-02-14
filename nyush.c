/*
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
https://stackoverflow.com/questions/30220691/how-to-get-empty-input-or-only-enter-in-c
Class Slides
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "cmdutils.h"

int main(){
    char* cmd = malloc(10000);
    char* cwd = malloc(10000);
    if(getcwd(cwd, 1000) == NULL) {
        printf("Error: Unable to find the cwd");
        return -1;
    }
    char* dirName = getDirName(cwd);
    printf("[nyush %s]$ ", dirName);
    fflush(stdout);
    while(fgets(cmd, sizeof cmd, stdin) != NULL){
        if(!validateCmd(cmd)){
            printf("Error: invalid command\n");
        }
        printf("[nyush %s]$ ", dirName);
        fflush(stdout);
    }
    free(cwd);
    free(cmd);
    return 0;
}