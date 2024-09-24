#include <stdlib.h>
#include <stdio.h>
#include "validations.h"
#include "common.h"

void generate_password(char *str, int len)
{
    int i = 0;

    for (; i < len; i++)
    {
        str[i] = (rand() % 100) + 1;
        str[i + 1] = '\0';
        while (is_end_of_string_char(str[i]) || !validate_password(str))
            str[i] = rand();
    }
    str[i] = '\0';
}
