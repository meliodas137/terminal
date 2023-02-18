/*
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
https://stackoverflow.com/questions/30220691/how-to-get-empty-input-or-only-enter-in-c
https://www.educative.io/answers/how-to-write-regular-expressions-in-c
https://www.w3schools.com/c/c_strings.php
https://www.asciitable.com/
https://pubs.opengroup.org/onlinepubs/009695399/functions/exit.html
https://man7.org/linux/man-pages/man3/errno.3.html
https://linuxhint.com/signal_handlers_c_programming_language/
Class Slides
*/
#include "executor.h"
#include "cmdutils.h"

int main(){
    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    char* cmd = malloc(1000);
    char* cwdBuffer = malloc(1000);
    char* dirName = getcurDir(cwdBuffer);
    printf("[nyush %s]$ ", dirName);
    fflush(stdout);
    while(fgets(cmd, 1000, stdin) != NULL){
        executeCmd(cmd);
        dirName = getcurDir(cwdBuffer);        
        printf("[nyush %s]$ ", dirName);
        fflush(stdout);
    }
    free(cmd);
    free(cwdBuffer);
    return 0;
}