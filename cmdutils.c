#include "cmdutils.h"

bool validateCmd(char** cmd){
    int idx = 0;
    regex_t notAllowed;
    regcomp(&notAllowed, "[\40\11\76\74\174\52\41\140\47\42]", REG_EXTENDED);
    int endState = -1, psuedoEnd = -2, currState = 0, terminate = 6, terminateStart = 7;
    while(cmd[idx] != NULL) {
        if(currState == endState) return false;
        if(currState == 0 && (strcmp(cmd[idx], "cd") == 0 || strcmp(cmd[idx], "fg") == 0)) currState = 1;
        else if(currState == 0 && (strcmp(cmd[idx], "exit") == 0 || strcmp(cmd[idx], "jobs") == 0)) currState = -1;
        else if(currState == 0 && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH) currState = -2;
        else if(currState == 1 && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH) currState = endState;
        else if(currState == -2 && strcmp(cmd[idx], "<") == 0) currState = 2;
        else if((currState == -2 || currState == terminate) && strcmp(cmd[idx], "|") == 0) currState = 3;
        else if(currState == -2 && (strcmp(cmd[idx], ">>") == 0 || strcmp(cmd[idx], ">") == 0)) currState = 4;
        else if(currState == terminate && (strcmp(cmd[idx], ">>") == 0 || strcmp(cmd[idx], ">") == 0)) currState = 8;
        else if((currState == -2 || currState == terminate)  && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH);
        else if(currState == 2 && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH) currState = 5;
        else if(currState == 3 && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH){
            if(strcmp(cmd[idx], "cd") == 0 || strcmp(cmd[idx], "fg") == 0
                || strcmp(cmd[idx], "exit") == 0 || strcmp(cmd[idx], "jobs") == 0) return false;
            currState = terminate;
        }
        else if(currState == 4 && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH) currState = terminateStart;
        else if(currState == 5 && strcmp(cmd[idx], "|") == 0) currState = 3;
        else if(currState == 5 && (strcmp(cmd[idx], ">>") == 0 || strcmp(cmd[idx], ">") == 0)) currState = 8;
        else if(currState == terminateStart && strcmp(cmd[idx], "<") == 0) currState = 8;
        else if(currState == 8 && regexec(&notAllowed, cmd[idx], 0, NULL, 0) == REG_NOMATCH) currState = endState;
        else return false;
        idx++;
    }
    if(idx == 0 || endState == currState || currState == psuedoEnd || currState == terminate || currState == terminateStart || currState == 5) return true;
    return false;
}

char* getcurDir(char* buffer) {
    if(getcwd(buffer, 1000) == NULL) {
        printf("Error: Unable to find the cwd");
        return buffer;
    }
    return getDirName(buffer);
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
    char** cmdMap;
    int countCmdParts = 0;
    if(cmd[0] == '\n' || cmd[0] == '\0'){
        cmdMap = malloc(1);
    }
    else {
        int len = 0; countCmdParts++;
        while(cmd[len] != '\0' && cmd[len] != '\n') if(cmd[len++] == ' ') countCmdParts++;
        cmdMap = malloc((countCmdParts+1)*sizeof(char*));
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
    }
    
    cmdMap[countCmdParts] = NULL;
    return cmdMap;
}

void freeStrMap(char** cmdMap){
    int idx = 0;
    while(cmdMap[idx] != NULL) free(cmdMap[idx++]);
    free(cmdMap);
    return;
}

char* getSanitizedCmd(char* cmd){
    if(cmd[0] == '/' || cmd[0] == '.') return cmd;
    int idx = 0;
    char* newCmd;
    while(cmd[idx] != '\0') if(cmd[idx++] == '/') {
        newCmd = malloc(strlen(cmd)+2);
        strcpy(newCmd, "./");
        strcat(newCmd, cmd);
        free(cmd);
        return newCmd;
    }
    return cmd;
}

char** removeFromMap(char** map, int idx){ 
    char* cmd = map[idx];
    char** argv = splitCmd(cmd);
    while(map[idx+1] != NULL) {
        map[idx] = map[idx+1];
        idx++;
    }
    free(cmd);
    map[idx] = NULL;
    return argv;
}

void putInMap(char** map, int pid, char** argv) {
    char* s_pid = malloc(1000);
    if(pid != -1) {
        sprintf(s_pid, "%d", pid);
        strcat(s_pid, " ");
    }
    int idx = 0;
    while(argv[idx] != NULL) {
        strcat(s_pid, argv[idx++]);
        if(argv[idx] != NULL) strcat(s_pid, " ");
    }
    idx = -1;
    while(map[++idx] != NULL);
    map[idx] = s_pid;
    map[++idx] = NULL;
    return;
}
