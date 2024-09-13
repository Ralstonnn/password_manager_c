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

int search_records(int (*search_algo)(char *line, char *search_str), char *str, int size, struct account_record *records)
{
    FILE *file;

    file = fopen(FILE_PATH, "r");

    if (!file)
        return 0;

    int i = 0;
    int line_len = USERNAME_BUFFER_SIZE * 2 + PASSWORD_BUFFER_SIZE;
    char line[line_len];
    struct account_record current_record;

    while (fgets(line, line_len, file) != NULL && i < size)
    {
        current_record = parse_string_to_record(line);
        if (search_algo(current_record.name, str))
            records[i++] = current_record;
    }

    fclose(file);
    return i;
}
