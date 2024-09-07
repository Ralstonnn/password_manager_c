#include <stdio.h>
#include "common.h"
#include "fs.h"

#define HEADER_LENGTH 40
#define HEADER_HEIGHT 3

static const char HEADER[HEADER_HEIGHT][HEADER_LENGTH] = {
    "########################################",
    "#            PM (NOT NADYA)            #",
    "########################################"};

void clear_screen();
void print_initial_screen();
struct account_record create_user();

void print_interface()
{
    char ch;

    print_initial_screen();
    printf(": ");

    while (1)
    {
        scanf("%s", &ch);
        getchar();

        switch (ch)
        {
        case 'c':
            clear_screen();
            create_user();
            break;
        case 'q':
            break;
        default:
            print_initial_screen();
            break;
        }

        if (ch != 'q')
            printf(": ");
        else
            break;
    }
}

struct account_record create_user()
{
    struct account_record record;

    printf("name (%d): ", USERNAME_BUFFER_SIZE - 1);
    get_string(record.name, USERNAME_BUFFER_SIZE);
    trim_whitespaces(record.name);
    printf("name: %s\n\n", record.name);

    printf("username (%d): ", USERNAME_BUFFER_SIZE - 1);
    get_string_no_whitespaces(record.username, USERNAME_BUFFER_SIZE);
    printf("username: %s\n\n", record.username);

    printf("password (%d): ", PASSWORD_BUFFER_SIZE - 1);
    get_string_no_whitespaces(record.password, PASSWORD_BUFFER_SIZE);
    printf("password: %s\n\n", record.password);

    int is_saved = save_record(record);

    if (is_saved == 0) 
        printf("Record saved successfully\n");
    else 
        printf("Something went wrong\n");

    printf("Press any key to continue\n\n");

    getchar();

    print_initial_screen();

    return record;
}

void print_initial_screen()
{
    clear_screen();
    for (int i = 0; i < HEADER_HEIGHT; i++)
    {
        for (int j = 0; j < HEADER_LENGTH; j++)
        {
            printf("%c", HEADER[i][j]);
        }
        printf("\n");
    }
    printf("\nChoose an option\n");
    printf("> (c) create\n");
    printf("> (q) quit\n");
    printf("\n");
}

void clear_screen()
{
    printf("\e[1;1H\e[2J");
    fflush(stdout);
}
