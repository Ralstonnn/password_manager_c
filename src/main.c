#include "db.h"
#include "encryption.h"
#include "interface.h"
#include "paths.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // Called for rand() to work
    srand(time(0));

    if (!init_default_paths()) {
        printf("Failed to create directory for program files");
        return 1;
    }
    if (!init_db()) {
        printf("Failed to initialize database\n");
        return 1;
    }
    if (!encryption_init()) {
        printf("Failed to initialize encryption\n");
        return 1;
    }

    print_interface();

    return 0;
}
