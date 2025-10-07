#include "../../../libc/include/stdio.h"

#define MAX_ARG_LEN 256

typedef enum BASH_COMMAND_LIST_EN {
    BASH_ECHO = 0,
} BASH_COMMAND_LIST;

typedef struct BASH_COMMAND_ST {
    uint16_t id;
    char args[MAX_ARG_LEN];
} BASH_COMMAND;


BASH_COMMAND bash_cmd_echo(char args[MAX_ARG_LEN]);

int bash_execute(BASH_COMMAND cmd);