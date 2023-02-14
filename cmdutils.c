#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "cmdutils.h"

bool validateCmd(char* cmd){
    if(cmd[0] == '\n') return true;
    return true;
}

char* getDirName(char* path){
    int len = -1;
    while(path[++len] != '\0');
    if(len == 1) return path;
    int start = len;
    while(path[--start] != '/');
    return path+start+1;
}

char** splitCmd(char* cmd) {
    int countCmdParts = 1;
    int len = 0;
    while(cmd[len] != '\0' && cmd[len] != '\n') if(cmd[len++] == ' ') countCmdParts++;
    char** cmdMap = malloc((countCmdParts+1)*sizeof(char*));
    int start = 0, currItem = 0;
    for(int i = 0; i<=len; i++){
        if(cmd[i] == ' ' || cmd[i] == '\0' || cmd[i] == '\n'){
            cmdMap[currItem] = malloc(i-start+1);
            int j = 0;
            while(start<i) cmdMap[currItem][j++] = cmd[start++];
            cmdMap[currItem][j] = '\0';
            currItem++;
            start++;
        }
    }

    cmdMap[countCmdParts] = NULL;
    return cmdMap;
}