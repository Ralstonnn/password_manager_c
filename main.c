#include <stdio.h>
#include "common.h"
#include "interface/interface.h"

struct account_record split(char *str);

int main()
{
    print_interface();
    return 0;
}

void print_out_password()
{
    FILE *file;
    char str[MAX_BUFFER_SIZE];

    file = fopen("test.txt", "r");

    while (fgets(str, MAX_BUFFER_SIZE, file) != NULL)
    {
        struct account_record test = split(str);
        printf("name: %s\nuname: %s\npass: %s\n", test.name, test.username, test.password);
    }

    fclose(file);
}

struct account_record split(char *str)
{
    struct account_record record;
    int i = 0;

    // Find name
    for (; *str != ':'; str++)
    {
        record.name[i++] = *str;
    }

    // Set last character to the end of the string
    record.name[i] = '\0';
    // Increase str pointer to skip :
    str++;
    i = 0;

    // Find username
    for (; *str != ':'; str++)
    {
        record.username[i++] = *str;
    }

    // Set last character to the end of the string
    record.username[i] = '\0';
    // Increase str pointer to skip :
    str++;
    i = 0;

    // Find password
    for (; *str != '\0'; str++)
    {
        record.password[i++] = *str;
    }

    // Set last character to the end of the string
    record.password[i] = '\0';

    return record;
}

// void print_struct(struct account_record record) {

// }
