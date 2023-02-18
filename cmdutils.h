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
int removeFromMap(char** map, int id, int index, int map_size);
#endif
