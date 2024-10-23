#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Return a name of platform, if determined, otherwise - an empty string
const char *get_platform_name() { return PLATFORM_NAME; }

int is_wsl() {
    FILE *file;
    char buffer[256];

    file = fopen("/proc/version", "r");
    if (file == NULL) {
        return 0;
    }

    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    if (strstr(buffer, "microsoft") != NULL) {
        return 1;
    }
    return 0;
}
