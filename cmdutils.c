#include <stdio.h>
#include <stdbool.h>
#include "cmdutils.h"

bool validateCmd(char* cmd){
    if(cmd[0] == '\n') return true;
    return false;
}

char* getDirName(char* path){
    int len = -1;
    while(path[++len] != '\0');
    fflush(stdout);
    if(len == 1) return path;
    int start = len;
    while(path[--start] != '/');
    return path+start+1;
}