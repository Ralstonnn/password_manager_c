#include <stdio.h>
#include <stdlib.h>

void copy_to_clipboard(const char *text, int max_size)
{
    char command[max_size + 20];
    snprintf(command, sizeof(command), "echo '%s' | pbcopy", text);
    system(command);
}
