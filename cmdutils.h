#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef _CMDUTILS_H_
#define _CMDUTILS_H_

bool validateCmd(char** cmd);
char* getDirName(char* path);
char** splitCmd(char* cmd);
char* getcurDir(char* buffer);

#endif
