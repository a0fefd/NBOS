#include "include/bash.h"
#include "../../libc/include/string.h"

BASH_COMMAND bash_cmd_echo(char args[MAX_ARG_LEN])
{
    BASH_COMMAND echo = {
        BASH_ECHO
    };
    strcpy(echo.args, args, MAX_ARG_LEN);
    return echo;
}

int bash_execute(BASH_COMMAND cmd)
{
    int flag = 0;

    switch (cmd.id)
    {
        case BASH_ECHO:
            if (strlen(cmd.args) <= 0) { flag = 1;}
            puts(cmd.args);
            break;

        default:
            flag = -1;
            break;
    }

    return flag;
}

