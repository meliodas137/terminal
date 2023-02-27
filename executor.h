#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

void executeCmd(char* cmd);
int runCmd(char** cmdMap);
int handleBuiltInCmd(char** cmdMap);
int handlePrograms(char** cmdMap);
int sysCall(char** argv, char** argvd, int read, int write);
void suspendHandler(int signal);
void jobsHandler(int signal);
void childHandler(int signal);
void handleIORedirect(char** argv, int read, int write);
int extractCmdAndRun(char** cmdMap, int argc, int idx, int argd, int readFd, int writeFd);
void jobs();
void fg(char* jobNum);

#endif