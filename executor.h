#include <stdio.h>
#include <errno.h>

#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

void executeCmd(char* cmd);
int runCmd(char** cmdMap);
int handleBuiltInCmd(char** cmdMap);
int handlePrograms(char** cmdMap);
int sysCall(char** argv);

#endif