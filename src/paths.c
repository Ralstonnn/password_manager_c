#include "paths.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define PREFIX "/.local/share/passwordmanager"

char KEY_PATH[MAX_PATH_LEN];
char IV_PATH[MAX_PATH_LEN];
char DB_FILE_PATH[MAX_PATH_LEN];

int init_default_paths() {
    char BASE_PATH[MAX_PATH_LEN];
    snprintf(BASE_PATH, MAX_PATH_LEN, "%s%s", getenv("HOME"), PREFIX);

    struct stat sb;
    if (stat(BASE_PATH, &sb) == -1) {
        if (mkdir(BASE_PATH, 0755)) {
            return 0;
        }
    }

    snprintf(KEY_PATH, MAX_PATH_LEN, "%s/key", BASE_PATH);
    snprintf(IV_PATH, MAX_PATH_LEN, "%s/iv", BASE_PATH);
    snprintf(DB_FILE_PATH, MAX_PATH_LEN, "%s/db.db", BASE_PATH);

    return 1;
}
