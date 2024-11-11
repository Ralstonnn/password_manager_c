#include "common.h"
#include "copy_to_clipboard.h"
#include "db.h"
#include "password_generator.h"
#include "terminal.h"
#include "validations.h"
#include <stdio.h>

#define HEADER_LENGTH 40
#define HEADER_HEIGHT 3

static const char HEADER[HEADER_HEIGHT][HEADER_LENGTH] = {
    "########################################",
    "#            PM (NOT NADYA)            #",
    "########################################"};

void print_initial_screen();
void create_record();
void show_search_records();
void show_selected_record_screen(struct account_record record);
void print_selected_record_screen(struct account_record record,
                                  int is_password_hidden);
void show_generate_random_password();
int get_random_password_length(char *message);

void print_interface() {
    char ch;

    print_initial_screen();

    while (1) {
        enable_raw_mode();
        ch = getchar();

        switch (ch) {
        case 'c':
            create_record();
            break;
        case 's':
            show_search_records();
            break;
        case 'r':
            show_generate_random_password();
            break;
        case 'q':
        case EOF:
            break;
        default:
            print_initial_screen();
            break;
        }

        if (ch == 'q' || ch == EOF) {
            clear_screen();
            break;
        }
    }

    disable_raw_mode();
}

void create_record() {
    disable_raw_mode();
    clear_screen();
    struct account_record record = create_empty_account_record();
    int is_invalid = 0;

    printf("name (%d): ", USERNAME_BUFFER_SIZE - 1);
    get_string(record.name, USERNAME_BUFFER_SIZE);
    trim_whitespaces(record.name);

    while (1) {
        clear_screen();
        if (is_invalid) {
            is_invalid = 0;
            printf("The username was invalid\n");
            printf("Username should be no longer than %d characters\n",
                   USERNAME_BUFFER_SIZE - 1);
            printf("And can only contain this characters: A-Za-z0-9\n\n");
        }

        printf("name (%d): %s\n", USERNAME_BUFFER_SIZE - 1, record.name);
        printf("username (%d): ", USERNAME_BUFFER_SIZE - 1);
        get_string_no_whitespaces(record.username, USERNAME_BUFFER_SIZE);

        if (!validate_username(record.username) &&
            !is_string_empty(record.username)) {
            make_string_empty(record.username, USERNAME_BUFFER_SIZE);
            is_invalid = 1;
        } else {
            is_invalid = 0;
            break;
        }
    }

    while (1) {
        clear_screen();
        if (is_invalid) {
            is_invalid = 0;
            printf("The password was invalid\n");
            printf("Password should be no longer than %d characters\n",
                   PASSWORD_BUFFER_SIZE - 1);
            printf(
                "And can only contain this characters: A-Za-z0-9!@#$%%&*^\n\n");
        }

        printf("name (%d): %s\n", USERNAME_BUFFER_SIZE - 1, record.name);
        printf("username (%d): %s\n", USERNAME_BUFFER_SIZE - 1,
               record.username);
        printf("password (%d) <leave empty to generate random password>: ",
               PASSWORD_BUFFER_SIZE - 1);
        get_password_from_stdin(record.password, PASSWORD_BUFFER_SIZE);

        if (!validate_password(record.password) &&
            !is_string_empty(record.password)) {
            make_string_empty(record.password, PASSWORD_BUFFER_SIZE);
            is_invalid = 1;
        } else {
            is_invalid = 0;
            break;
        }
    }

    clear_screen();

    printf("name (%d): %s\n", USERNAME_BUFFER_SIZE - 1, record.name);
    printf("username (%d): %s\n", USERNAME_BUFFER_SIZE - 1, record.username);
    printf("password (%d) <leave empty to generate random password>: ",
           PASSWORD_BUFFER_SIZE - 1);
    printf("\n\n");

    enable_raw_mode();

    if (is_string_empty(record.name) || is_string_empty(record.username)) {
        clear_screen();
        printf("To save record provide name and username\n");
    } else if (is_string_empty(record.password)) {
        clear_screen();
        disable_raw_mode();
        generate_password(record.password, get_random_password_length(NULL));
        enable_raw_mode();
    }

    if (!is_string_empty(record.password) && !is_string_empty(record.name) &&
        !is_string_empty(record.username)) {
        int is_saved = save_record_db(&record);
        clear_screen();
        if (is_saved == 0)
            show_selected_record_screen(record);
        else
            printf("Something went wrong\n");
    }
}

void print_records(struct account_record *records, int records_size,
                   int selected_record_idx) {
    for (int i = 0; i < records_size; i++) {
        if (is_record_empty(records[i]))
            continue;

        if (i == selected_record_idx) {
            printf(">  Name: %s\n", records[i].name);
            printf("   Username: %s\n\n", records[i].username);
        } else {
            printf("   Name: %s\n", records[i].name);
            printf("   Username: %s\n\n", records[i].username);
        }
    }
}

void show_search_records() {
    clear_screen();
    enable_raw_mode();
    printf("Search: ");

    char search_str[56];
    char ch;
    int i, selected_record_idx, records_size_max, records_size;

    i = selected_record_idx = 0;
    records_size_max = 5;

    struct account_record records[records_size_max];

    while (!is_end_of_string_char((ch = getchar())) && i < 55) {
        hide_cursor();

        if (ch == '\x1b') {
            ch = getchar();
            if (ch == '[') {
                ch = getchar();
                switch (ch) {
                case 'A': // Up arrow
                    if (selected_record_idx > 0)
                        selected_record_idx--;
                    break;
                case 'B': // Down arrow
                    if (selected_record_idx < records_size - 1)
                        selected_record_idx++;
                    break;
                }
            }
        } else if (is_backspace_char(ch)) {
            if (i > 0)
                --i;
            search_str[i] = '\0';
        } else {
            search_str[i++] = ch;
            search_str[i] = '\0';
        }

        clear_screen();

        printf("Search: %s\n\n", search_str);
        fill_arrays_with_empty_records(records, records_size_max);
        search_records_db(records, search_str, records_size_max);
        records_size = get_records_size(records, records_size_max);
        print_records(records, records_size, selected_record_idx);

        set_cursor_position(0, 8 + i + 1);
        show_cursor();
    }

    search_str[i] = '\0';

    if (ch == '\n') {
        get_record_by_id_db(&records[selected_record_idx],
                            records[selected_record_idx].id);
        show_selected_record_screen(records[selected_record_idx]);
    } else {
        clear_screen();
        print_initial_screen();
        disable_raw_mode();
    }
}

void print_selected_record_hotkeys() {
    printf("Record selected \n\n");
    printf("Press \"u\" to copy username\n");
    printf("Press \"p\" to copy password\n");
    printf("Press \"h\" to show/hide password\n");
    printf("Press \"c\" to change record\n");
    printf("Press \"d\" to delete record\n");
    printf("Press \"q\" to quit to initial screen\n\n");
}

void print_selected_record_screen(struct account_record record,
                                  int is_password_hidden) {
    print_selected_record_hotkeys();
    printf("Record name: %s\n", record.name);
    printf("Username   : %s\n", record.username);

    if (is_password_hidden) {
        printf("Password   : ");
        for (int i = 0; !is_end_of_string_char(record.password[i]); i++)
            printf("*");
        printf("\n");
    } else {
        printf("Password   : %s\n", record.password);
    }
    printf("\n");
}

void print_change_record_screen(struct account_record record,
                                int is_changing_name, int is_changing_username,
                                int is_changing_password) {
    print_selected_record_hotkeys();

    if (is_changing_name)
        printf("Record name (press <Enter> to continue): %s\n", record.name);
    else
        printf("Record name: %s\n", record.name);
    if (is_changing_username)
        printf("Username    (press <Enter> to continue): %s\n",
               record.username);
    else
        printf("Username   : %s\n", record.username);

    if (is_changing_password)
        printf("Password    (press <Enter> to continue): %s\n",
               record.password);
    else
        printf("Password   : %s\n", record.password);

    printf("\n");
}

int change_record_screen(struct account_record *record) {
    enable_raw_mode();

    char ch, new_value[PASSWORD_BUFFER_SIZE + 5];
    int is_changing_name, is_changing_username, is_changing_password, len;

    const int prefix_len = 41;
    is_changing_name = 1;
    is_changing_username = is_changing_password = 0;

    while (1) {
        clear_screen();
        print_change_record_screen(*record, is_changing_name,
                                   is_changing_username, is_changing_password);

        if (is_changing_name) {
            len = get_str_length(record->name);
            set_cursor_position(10, prefix_len + len + 1);
        } else if (is_changing_username) {
            len = get_str_length(record->username);
            set_cursor_position(11, prefix_len + len + 1);
        } else {
            len = get_str_length(record->password);
            set_cursor_position(12, prefix_len + len + 1);
        }

        ch = getchar();

        if (is_escape_char(ch)) {
            break;
        } else if (ch == '\n') {
            if (is_changing_name) {
                is_changing_name = 0;
                is_changing_username = 1;
            } else if (is_changing_username) {
                is_changing_username = 0;
                is_changing_password = 1;
            } else {
                break;
            }
        } else if (is_backspace_char(ch)) {
            if (is_changing_name && len > 0) {
                record->name[len - 1] = '\0';
            } else if (is_changing_username && len > 0) {
                record->username[len - 1] = '\0';
            } else if (len > 0) {
                record->password[len - 1] = '\0';
            }
        } else {
            if (is_changing_name && len < USERNAME_BUFFER_SIZE) {
                record->name[len] = ch;
                record->name[len + 1] = '\0';
            } else if (is_changing_username && len < USERNAME_BUFFER_SIZE) {
                sprintf(new_value, "%s%c", record->username, ch);
                if (validate_username(new_value)) {
                    record->username[len] = ch;
                    record->username[len + 1] = '\0';
                }
            } else if (len < PASSWORD_BUFFER_SIZE) {
                sprintf(new_value, "%s%c", record->password, ch);
                if (validate_password(new_value)) {
                    record->password[len] = ch;
                    record->password[len + 1] = '\0';
                }
            }
        }
    }

    if (update_record_db(*record))
        return 0;

    struct account_record new_record;
    if (get_record_by_id_db(&new_record, record->id))
        return 0;

    *record = new_record;
    return 1;
}

void show_selected_record_screen(struct account_record record) {
    enable_raw_mode();
    clear_screen();

    int is_password_hidden = 1;
    int change_record_result;

    print_selected_record_screen(record, is_password_hidden);

    char ch;

    while ((ch = getchar()) != 'q') {
        switch (ch) {
        case 'h':
            is_password_hidden = !is_password_hidden;
            clear_screen();
            print_selected_record_screen(record, is_password_hidden);
            break;
        case 'u':
            copy_to_clipboard(record.username, USERNAME_BUFFER_SIZE);
            clear_screen();
            print_selected_record_screen(record, is_password_hidden);
            printf("Username copied");
            break;
        case 'p':
            copy_to_clipboard(record.password, PASSWORD_BUFFER_SIZE);
            clear_screen();
            print_selected_record_screen(record, is_password_hidden);
            printf("Password copied");
            break;
        case 'c':
            change_record_result = change_record_screen(&record);
            clear_screen();
            print_selected_record_screen(record, is_password_hidden);
            if (!change_record_result)
                printf("Something went wrong");
            else
                printf("Record was updated successfully");
            break;
        case 'd':
            printf("Are you sure you want to delete this record?\n");
            printf("Press <Enter> to continue or any other key to quit\n");
            char ch_del = getchar();

            if (ch_del != '\n') {
                clear_screen();
                print_selected_record_screen(record, is_password_hidden);
                break;
            }

            if (delete_record_db(record.id)) {
                printf("Something went wrong\nRecord was not deleted\nPress "
                       "any key to continue\n");
                break;
            } else {
                printf("\nRecord was deleted successfully!\nPress any key to "
                       "continue\n");
            }
            getchar();
            disable_raw_mode();
            clear_screen();
            print_initial_screen();
            return;
        default:
            break;
        }
    }

    disable_raw_mode();
    clear_screen();
    print_initial_screen();
}

int get_random_password_length(char *message) {
    char input[4];
    int number;
    do {
        if (message == NULL) {
            printf("Enter number of characters a password should have "
                   "(255): ");
        } else {
            printf("%s", message);
        }
        get_string(input, 4);
        number = string_to_int(input);
    } while (!is_number_string(input) || number > PASSWORD_BUFFER_SIZE - 1);
    return number;
}

void show_generate_random_password() {
    char password[PASSWORD_BUFFER_SIZE];
    for (int i = 0; i < PASSWORD_BUFFER_SIZE; i++)
        password[i] = '\0';

    char ch;
    int number;

    while (1) {
        disable_raw_mode();
        clear_screen();

        number = get_random_password_length(NULL);
        generate_password(password, number);
        enable_raw_mode();
        ch = '\0';

        do {
            clear_screen();
            printf("Password: %s\n\n", password);
            printf("(c) to copy password\n");
            printf("(r) to regenerate password\n");
            printf("(q) to quit\n\n");

            if (ch == 'c') {
                copy_to_clipboard(password, PASSWORD_BUFFER_SIZE);
                printf("Password copied successfully");
            } else if (ch == 'r') {
                break;
            }
        } while ((ch = getchar()) != 'q');

        if (ch == 'q')
            break;
    }

    disable_raw_mode();
    clear_screen();
    print_initial_screen();
}

void print_initial_screen() {
    clear_screen();
    for (int i = 0; i < HEADER_HEIGHT; i++) {
        for (int j = 0; j < HEADER_LENGTH; j++) {
            printf("%c", HEADER[i][j]);
        }
        printf("\n");
    }
    printf("\nChoose an option\n");
    printf("> (c) create\n");
    printf("> (s) search\n");
    printf("> (r) generate random password\n");
    printf("> (q) quit\n");
    printf("\n");
}
