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
int sysCall(char** argv, char** argvd);
void suspendHandler(int signal);
void putinJobsTable(int pid, char** argv);
void jobsHandler(int signal);
void childHandler(int signal);
void handleIORedirect(char** argv);
void jobs();
void fg(char* jobNum);

#endif