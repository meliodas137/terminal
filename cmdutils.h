#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#ifndef _CMDUTILS_H_
#define _CMDUTILS_H_

bool validateCmd(char** cmd);
char* getDirName(char* path);
char** splitCmd(char* cmd);
char* getcurDir(char* buffer);
void freeStrMap(char** cmdMap);
char* getSanitizedCmd(char* cmd);
void putInMap(char** map, int pid, char** argv);
char** removeFromMap(char** map, int idx);
#endif
