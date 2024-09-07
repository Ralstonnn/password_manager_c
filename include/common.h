#ifndef COMMON_H
#define COMMON_H

#define PASSWORD_BUFFER_SIZE 256
#define USERNAME_BUFFER_SIZE 56

#define MAX_LINE_SIZE 526

#define FILE_PATH "../test.txt"

struct account_record {
    char name[USERNAME_BUFFER_SIZE];
    char username[USERNAME_BUFFER_SIZE];
    char password[PASSWORD_BUFFER_SIZE];
};

int get_string(char* str, int size);
int get_string_no_whitespaces(char *str, int size);

void print_by_chars(char* str);

void trim(char *str, char ch);
void trim_whitespaces(char *str);

void copy_str(char *src, char *dest);
int get_str_length(char *str);
int is_end_of_string_char(char ch);

#endif