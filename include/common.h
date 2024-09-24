#ifndef COMMON_H
#define COMMON_H

#define PASSWORD_BUFFER_SIZE 256
#define USERNAME_BUFFER_SIZE 56

#define MAX_LINE_SIZE 526

#define FILE_PATH "../test.txt"

struct account_record
{
    char name[USERNAME_BUFFER_SIZE];
    char username[USERNAME_BUFFER_SIZE];
    char password[PASSWORD_BUFFER_SIZE];
};

struct account_record create_empty_account_record();
void fill_arrays_with_empty_records(struct account_record *records, int size);
struct account_record parse_string_to_record(char *str);
int is_record_empty(struct account_record record);

int get_string(char *str, int size);
int get_string_no_whitespaces(char *str, int size);
int is_char_int_string(char *str, char ch);

void print_by_chars(char *str);

void trim(char *str, char ch);
void trim_whitespaces(char *str);
int is_backspace_char(char ch);
int is_english_letter(char ch);

void copy_str(char *src, char *dest);
int get_str_length(char *str);
int is_end_of_string_char(char ch);
int is_string_empty(char *str);
int string_to_int(char *str);

void clear_screen();
void set_cursor_position(int row, int col);
void hide_cursor();
void show_cursor();

#endif