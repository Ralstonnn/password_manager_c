#include <stdio.h>
#include "common.h"

int is_end_of_string_char(char ch)
{
    return ch == EOF || ch == '\0' || ch == '\n';
}

int get_str_length(char *str)
{
    int len = 0;
    while (!is_end_of_string_char(str[len]))
        len++;
    len++;
    return len;
}

void copy_str(char *src, char *dest)
{
    while (!is_end_of_string_char(*src))
        *dest++ = *src++;
    *dest = '\0';
}

void trim(char *str, char ch)
{
    int len = get_str_length(str);
    int end_trim = len - 1;
    int start_trim = 0;

    char copy[len];
    copy_str(str, copy);

    for (int i = len - 2; i >= 0 && copy[i] == ch; i--, end_trim--)
        ;
    for (; start_trim < end_trim && copy[start_trim] == ch; start_trim++)
        ;

    for (int i = 0; i < len; i++)
    {
        if (start_trim + i < end_trim)
            str[i] = copy[start_trim + i];
        else
            str[i] = '\0';
    }
}

void trim_whitespaces(char *str)
{
    trim(str, ' ');
}

int get_string(char *str, int size)
{
    int len = 0;

    for (char ch; (ch = getchar()) != EOF && ch != '\n';)
    {
        if (len < size - 1)
            str[len++] = ch;
    }

    str[len++] = '\0';
    return len;
}

int get_string_no_whitespaces(char *str, int size)
{
    int len = 0;

    for (char ch; (ch = getchar()) != EOF && ch != '\n';)
    {
        if (ch == ' ')
            continue;
        if (len < size - 1)
            str[len++] = ch;
    }

    str[len++] = '\0';
    return len;
}

void print_by_chars(char *str)
{
    while (!is_end_of_string_char(*str))
        printf("%c", *str++);

    if (*str == '\n')
        printf("\\n");
    else if (*str == '\0')
        printf("\\0");
    else if (*str == EOF)
        printf("EOF");
}
