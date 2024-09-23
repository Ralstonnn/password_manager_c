#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "common.h"
#include "interface.h"

struct account_record split(char *str);

int main()
{
    // Called for rand() to work
    srand(time(0));
    
    print_interface();
    return 0;
}
