#include "../include/db.h"
#include <stdio.h>

#define JSON_STRING_MAX 4096

int read_json(char *out);
int parse(char *json_str);

/*
**  IMPORT
*/
int import_json() {
    char json_str[JSON_STRING_MAX];
    if (!read_json(json_str)) {
        return 0;
    }
    parse(json_str);
    printf("%s\n", json_str);
    return 1;
}

int read_json(char *out) {
    FILE *file = fopen("../import.json", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 0;
    }
    fread(out, 1, JSON_STRING_MAX, file);
    fclose(file);
    return 1;
}

/*
**  PARSER
*/
int parse(char *json_str) {
    int i = 0;
    return 0;
}
