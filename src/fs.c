#include <stdio.h>
#include "common.h"

int save_record(struct account_record record)
{
    FILE *file;
    file = fopen(FILE_PATH, "a");

    if (!file)
        return 1;
    
    fprintf(file, "%s:%s:%s\n", record.name, record.username, record.password);
    fclose(file);
    
    return 0;
}
