#include "common.h"
#include "terminal.h"
#include <stdio.h>

struct account_record create_empty_account_record() {
    struct account_record record = {-1, "\0", "\0", "\0"};
    return record;
}

void fill_arrays_with_empty_records(struct account_record *records, int size) {
    for (int i = 0; i < size; i++) {
        records[i].name[0] = '\0';
        records[i].username[0] = '\0';
        records[i].password[0] = '\0';
    }
}

struct account_record parse_string_to_record(char *str) {
    struct account_record record = create_empty_account_record();

    int i = 0;
    while (*str != ':')
        record.name[i++] = *str++;
    str++;
    i = 0;
    while (*str != ':')
        record.username[i++] = *str++;
    str++;
    i = 0;
    while (*str != ':' && !is_end_of_string_char(*str))
        record.password[i++] = *str++;

    return record;
}

int is_record_empty(struct account_record record) {
    return is_string_empty(record.name) && is_string_empty(record.username) &&
           is_string_empty(record.password);
}

int get_records_size(struct account_record *records, int max_size) {
    int i;
    for (i = 0; !is_record_empty(records[i]) && i < max_size; i++)
        ;
    return i;
}

int is_end_of_string_char(char ch) {
    return ch == EOF || ch == '\0' || ch == '\n';
}

int is_char_int_string(char *str, char ch) {
    while (!is_end_of_string_char(*str++))
        if (*str == ch)
            return 1;

    return 0;
}

int is_backspace_char(char ch) { return ch == '\b' || ch == 127; }
int is_escape_char(char ch) { return ch == 27; }

int is_english_letter(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

int get_str_length(char *str) {
    if (is_end_of_string_char(*str)) {
        return 0;
    }

    int len = 0;
    while (!is_end_of_string_char(str[len]))
        len++;

    return len;
}

void copy_str(char *src, char *dest) {
    while (!is_end_of_string_char(*src))
        *dest++ = *src++;
    *dest = '\0';
}

int is_string_empty(char *str) {
    while (!is_end_of_string_char(*str++)) {
        if (*str != ' ' && *str != '\t')
            return 0;
    }

    return 1;
}

void make_string_empty(char *str, int size) {
    for (int i = 0; i < size; i++)
        str[i] = '\0';
}

int are_equal_strings(char *str1, char *str2) {
    int i = 0;
    while (!is_end_of_string_char(str1[i]) && !is_end_of_string_char(str2[i])) {
        if (str1[i] != str2[i]) {
            return 0;
        }
        i++;
    }
    if (str1[i] != str2[i]) {
        return 0;
    }
    return 1;
}

void trim(char *str, char ch) {
    int len = get_str_length(str);
    int end_trim = len - 1;
    int start_trim = 0;

    char copy[len];
    copy_str(str, copy);

    for (int i = len - 1; i >= 0 && copy[i] == ch; i--, end_trim--)
        ;
    for (; start_trim < end_trim && copy[start_trim] == ch; start_trim++)
        ;

    for (int i = 0; i < len; i++) {
        if (start_trim + i <= end_trim)
            str[i] = copy[start_trim + i];
        else
            str[i] = '\0';
    }
}

void trim_whitespaces(char *str) { trim(str, ' '); }

int get_string(char *str, int size) {
    int len = 0;

    for (char ch; (ch = getchar()) != EOF && ch != '\n' && len < size - 1;
         len++)
        str[len] = ch;

    str[len] = '\0';
    return len;
}

int get_string_no_whitespaces(char *str, int size) {
    int len = 0;
    char ch;

    while ((ch = getchar()) != EOF && ch != '\n' && len < size - 1) {
        if (ch == ' ') {
            continue;
        } else {
            str[len++] = ch;
        }
    }

    str[len] = '\0';
    return len;
}

int get_password_from_stdin(char *out, int size) {
    char ch;
    int i;

    for (int i = 0; i < size; i++)
        out[i] = '\0';

    enable_raw_mode();

    i = 0;
    while (!is_end_of_string_char((ch = getchar())) && i < size - 1) {
        if (ch == ' ') {
            continue;
        } else if (is_backspace_char(ch)) {
            if (i > 0) {
                --i;
                printf("\b \b");
            }
            out[i] = '\0';
        } else {
            out[i++] = ch;
            printf("*");
        }
    }
    out[i] = '\0';

    disable_raw_mode();
    return i;
}

void print_by_chars(char *str) {
    while (!is_end_of_string_char(*str))
        printf("%c", *str++);

    if (*str == '\n')
        printf("\\n");
    else if (*str == '\0')
        printf("\\0");
    else if (*str == EOF)
        printf("EOF");
}

int string_to_int(char *str) {
    int result = 0;
    while (!is_end_of_string_char(*str)) {
        result += *str++ - '0';
        result *= 10;
    }
    return result / 10;
}

void clear_screen() { printf("\033[H\033[J"); }

void set_cursor_position(int row, int col) { printf("\033[%d;%dH", row, col); }

void hide_cursor() { printf("\033[?25l"); }

void show_cursor() { printf("\033[?25h"); }

void print_in_bibary(char *str, int len) {
    // Equals 10000000 in binary
    int mask = 0x80;

    for (int i = 0; i < len; i++) {
        char current = str[i];
        for (int j = 0; j < 8; j++) {
            int bit = (current & mask) ? 1 : 0;
            current <<= 1;
            printf("%i", bit);
        }
        printf("\n");
    }

    printf("\n%s\n\n", str);
}
