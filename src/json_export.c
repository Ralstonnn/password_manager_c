#include "common.h"
#include "db.h"
#include <stdio.h>

int write_items(const char *path);
int write_initial_template(const char *path);
int write_ending_template(const char *path);

int json_export(const char *path) {
    if (!write_initial_template(path))
        return 0;
    if (!write_items(path))
        return 0;
    if (!write_ending_template(path))
        return 0;

    return 1;
}

int write_items(const char *path) {
    const int records_count = get_records_count_db();
    if (!records_count)
        return 1;

    FILE *file = fopen(path, "a");
    if (file == NULL)
        return 0;

    char template[2000];
    struct account_record record;

    for (int i = 0; i < records_count; i++) {
        if (get_record_with_offset_db(&record, 1, i))
            continue;

        if (i == records_count - 1) {
            sprintf(template,
                    "\t\t{\n"
                    "\t\t\t\"name\": \"%s\",\n"
                    "\t\t\t\"username\": \"%s\",\n"
                    "\t\t\t\"password\": \"%s\"\n"
                    "\t\t}\n",
                    record.name, record.username, record.password);
        } else {
            sprintf(template,
                    "\t\t{\n"
                    "\t\t\t\"name\": \"%s\",\n"
                    "\t\t\t\"username\": \"%s\",\n"
                    "\t\t\t\"password\": \"%s\"\n"
                    "\t\t},\n",
                    record.name, record.username, record.password);
        }

        fprintf(file, "%s", template);
    }

    fclose(file);
    return 1;
}

int write_initial_template(const char *path) {
    FILE *file = fopen(path, "w+");
    if (file == NULL) {
        return 0;
    }
    const char template[14] = "{\n\t\"items\": [\n";
    fprintf(file, "%s", template);
    fclose(file);
    return 1;
}

int write_ending_template(const char *path) {
    FILE *file = fopen(path, "a");
    if (file == NULL) {
        return 0;
    }
    const char template[4] = "\t]\n}";
    fprintf(file, "%s", template);
    fclose(file);
    return 1;
}