#ifndef PATHS_H
#define PATHS_H

#define MAX_PATH_LEN 512

extern char KEY_PATH[MAX_PATH_LEN];
extern char IV_PATH[MAX_PATH_LEN];
extern char DB_FILE_PATH[MAX_PATH_LEN];

int init_default_paths();

#endif
