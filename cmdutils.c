#include <stdbool.h>
#include "cmdutils.h"

bool validateCmd(char* cmd){
    if(cmd[0] == '\n') return true;
    return false;
}