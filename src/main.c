#include "common.h"
#include "encryption.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct account_record split(char *str);

int main() {
    // Called for rand() to work
    srand(time(0));

    if (!encryption_init()) {
        printf("Failed to initialize encryption\n");
        return 1;
    }
    print_interface();

    return 0;
}
