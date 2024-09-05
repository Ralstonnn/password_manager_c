#include <stdio.h>

#define HEADER_LENGTH 40
#define HEADER_HEIGHT 3

static const char HEADER[HEADER_HEIGHT][HEADER_LENGTH] = {
    "########################################",
    "#            PM (NOT NADYA)            #",
    "########################################"};

void clear_screen();
void print_initial_screen();

void print_interface()
{
    char ch;
    int run_loop = 1;

    print_initial_screen();
    printf(": ");

    while (run_loop)
    {
        scanf("%s", &ch);

        switch (ch)
        {
        case 'c':
            clear_screen();
            printf("creating");
            break;
        case 'q':
            run_loop = 0;
            break;
        default:
            print_initial_screen();
            break;
        }

        if (ch != 'q')
        {
            printf(": ");
        }
    }
}

void clear_screen()
{
    printf("\e[1;1H\e[2J");
    fflush(stdout);
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