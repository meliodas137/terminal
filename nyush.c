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
    while(1){
        char* cwd = malloc(1000);
        if(getcwd(cwd, 1000) == NULL) {
            printf("Error: Unable to find the cwd");
            return -1;
        }; 
        if(cwd[1] != '\0') {
            cwd = cwd + 1;
        }
        printf("[nyush %s]$ ", cwd);
        fflush(stdout);
        char cmd[1000];
        fgets(cmd, sizeof cmd, stdin);
        if(!validateCmd(cmd)){
            printf("Error: invalid command\n");
        }
    }
    return 0;
}