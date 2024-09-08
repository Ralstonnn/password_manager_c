#include <stdio.h>
#include "common.h"
#include "fs.h"
#include "terminal.h"

#define HEADER_LENGTH 40
#define HEADER_HEIGHT 3

static const char HEADER[HEADER_HEIGHT][HEADER_LENGTH] = {
    "########################################",
    "#            PM (NOT NADYA)            #",
    "########################################"};

void clear_screen();
void print_initial_screen();
struct account_record create_record();
void show_search_records();

void print_interface()
{
    char ch;

    print_initial_screen();
    printf(": ");

    while (1)
    {
        scanf("%s", &ch);
        char ch2 = getchar();

        if (ch2 == EOF)
        {
            clear_screen();
            break;
        }

        switch (ch)
        {
        case 'c':
            create_record();
            break;
        case 's':
            show_search_records();
            break;
        case 'q':
        case EOF:
            break;
        default:
            print_initial_screen();
            break;
        }

        if (ch != 'q' && ch != EOF)
            printf(": ");
        else
        {
            clear_screen();
            break;
        }
    }
}

struct account_record create_record()
{
    clear_screen();
    struct account_record record = create_empty_account_record();

    printf("name (%d): ", USERNAME_BUFFER_SIZE - 1);
    get_string(record.name, USERNAME_BUFFER_SIZE);
    trim_whitespaces(record.name);

    printf("username (%d): ", USERNAME_BUFFER_SIZE - 1);
    get_string_no_whitespaces(record.username, USERNAME_BUFFER_SIZE);

    printf("password (%d): ", PASSWORD_BUFFER_SIZE - 1);
    get_string_no_whitespaces(record.password, PASSWORD_BUFFER_SIZE);

    printf("\n");

    // If all fields are empty - skip saving
    if (is_string_empty(record.name) && is_string_empty(record.username) && is_string_empty(record.password))
    {
        printf("To save record provide at least one field\n");
    }
    else
    {

        int is_saved = save_record(record);
        if (is_saved == 0)
            printf("Record saved successfully\n");
        else
            printf("Something went wrong\n");
    }

    printf("Press any key to continue\n\n");

    getchar();

    print_initial_screen();

    return record;
}

void show_search_records()
{
    clear_screen();
    printf("Search: ");

    char search_str[56];
    char ch;
    int i, selected_record_idx;
    i = selected_record_idx = 0;

    while (!is_end_of_string_char((ch = getchar())) && i < 55)
        search_str[i++] = ch;
    search_str[i] = '\0';
    printf("\n");

    int records_size = 5;
    struct account_record records[records_size];
    fill_arrays_with_empty_records(records, records_size);
    search_records(search_str, records_size, records);

print_records:
    for (int i = 0; i < records_size; i++)
    {
        if (is_record_empty(records[i]))
            continue;

        if (i == selected_record_idx)
        {
            printf(">  Name: %s\n", records[i].name);
            printf("   Username: %s\n\n", records[i].username);
        }
        else
        {
            printf("   Name: %s\n", records[i].name);
            printf("   Username: %s\n\n", records[i].username);
        }
    }

    enable_raw_mode();
    while (1)
    {
        ch = getchar();
        if (ch == '\x1b')
        {
            ch = getchar();
            if (ch == '[')
            {
                ch = getchar();
                switch (ch)
                {
                case 'A': // Up arrow
                    if (selected_record_idx > 0)
                    {
                        selected_record_idx--;
                        disable_raw_mode();
                        clear_screen();
                        printf("Press \"q\" to quit select mode\n\n");
                        goto print_records;
                    }
                    break;
                case 'B': // Down arrow
                    selected_record_idx++;
                    disable_raw_mode();
                    clear_screen();
                    printf("Press \"q\" to quit select mode\n\n");
                    goto print_records;
                    break;
                }
            }
        }

        if (ch == 'q')
        {
            disable_raw_mode();
            clear_screen();
            print_initial_screen();
            break;
        }
    }
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
    printf("> (s) search\n");
    printf("> (q) quit\n");
    printf("\n");
}

void clear_screen()
{
    printf("\e[1;1H\e[2J");
    fflush(stdout);
}
