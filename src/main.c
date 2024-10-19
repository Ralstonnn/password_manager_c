#include "common.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct account_record split(char *str);

int main()
{
    // Called for rand() to work
    srand(time(0));

    print_interface();
    return 0;
}
