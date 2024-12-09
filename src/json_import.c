#include "common.h"
#include "db.h"
#include <stdio.h>
#include <validations.h>

#define ITEMS_KEY_NAME "items"
#define ITEM_NAME_KEY_NAME "name"
#define ITEM_LOGIN_KEY_NAME "login"
#define ITEM_LOGIN_USERNAME_KEY_NAME "username"
#define ITEM_LOGIN_PASSWORD_KEY_NAME "password"

int parse();
int handle_items_value();
int handle_item_login_value(struct account_record *record);
int skip_value();
int get_json_string_value(char *out);
int skip_json_string();
void skip_blank_characters();
char peek();
char consume();

FILE *file;

/*
**  IMPORT
*/
int json_import(const char *path) {
    file = fopen(path, "r");
    if (file == NULL)
        return 0;
    parse();
    fclose(file);
    return 1;
}

/*
**  PARSER
*/
int parse() {
    char ch = consume();
    if (ch != '{')
        return 0;

    skip_blank_characters();
    ch = peek();

    if (ch == EOF || ch == '}')
        return 0;

    int items_found = 0;
    char key_name[256];

    while (ch != EOF) {
        ch = peek();

        switch (ch) {
        case '"':
            if (!get_json_string_value(key_name))
                return 0;
            if (are_equal_strings(key_name, ITEMS_KEY_NAME)) {
                items_found = 1;
                if (!handle_items_value())
                    return 0;
            } else if (!skip_value()) {
                return 0;
            }
            break;
        default:
            consume();
            break;
        }
    }

    return items_found;
}

int handle_items_value() {
    skip_blank_characters();
    char ch = peek();
    if (ch != ':')
        return 0;
    consume();

    struct account_record record;
    int nesting_index = 0;
    int item_nesting_depth = 0;
    char nesting[64];
    char key_name[256];

    for (int i = 0; i < 64; ++i)
        nesting[i] = '\0';

    skip_blank_characters();
    ch = peek();
    if (ch != '[')
        return 0;
    consume();
    nesting[nesting_index++] = ch;

    while (nesting[0] != '\0' && ch != EOF) {
        ch = peek();
        switch (ch) {
        case '"':
            if (!get_json_string_value(key_name))
                return 0;
            if (are_equal_strings(key_name, ITEM_NAME_KEY_NAME)) {
                while ((ch = peek()) == '\n' || ch == '\t' || ch == ' ' ||
                       ch == ':') {
                    consume();
                }
                char value[512];
                if (!get_json_string_value(value))
                    return 0;
                copy_str(value, record.name);
            } else if (are_equal_strings(key_name, ITEM_LOGIN_KEY_NAME)) {
                if (!handle_item_login_value(&record))
                    return 0;
            } else if (!skip_value()) {
                return 0;
            }
            break;
        case '[':
        case '{':
            if (ch == '{' && item_nesting_depth) {
                return 0;
            }
            nesting[nesting_index++] = ch;
            item_nesting_depth++;
            consume();
            break;
        case '}':
        case ']':
            nesting_index--;
            item_nesting_depth--;
            if ((ch == ']' && nesting[nesting_index] == '[') ||
                (ch == '}' && nesting[nesting_index] == '{')) {
                nesting[nesting_index] = '\0';
                consume();
            } else {
                return 0;
            }

            if (item_nesting_depth == 0 && !is_record_empty(record)) {
                if (save_record_db(&record))
                    return 0;
                record = create_empty_account_record();
            }
            break;
        default:
            consume();
            break;
        }
    }

    return 1;
}

int handle_item_login_value(struct account_record *record) {
    skip_blank_characters();
    char ch = peek();
    if (ch != ':')
        return 0;
    consume();

    skip_blank_characters();
    ch = peek();
    int nesting_level = 0;
    char key_name[256];

    if (ch != '{')
        return 0;
    consume();
    nesting_level++;

    while (nesting_level > 0 && ch != EOF) {
        ch = peek();
        switch (ch) {
        case '"':
            if (!get_json_string_value(key_name))
                return 0;
            if (are_equal_strings(key_name, ITEM_LOGIN_USERNAME_KEY_NAME) ||
                are_equal_strings(key_name, ITEM_LOGIN_PASSWORD_KEY_NAME)) {
                while ((ch = peek()) == '\n' || ch == '\t' || ch == ' ' ||
                       ch == ':') {
                    consume();
                }
                char value[512];
                make_string_empty(value, 512);
                get_json_string_value(value);
                if (are_equal_strings(key_name, ITEM_LOGIN_USERNAME_KEY_NAME)) {
                    copy_str(value, record->username);
                } else {
                    copy_str(value, record->password);
                }
            } else if (!skip_value()) {
                return 0;
            }
            break;
        case '[':
        case '{':
            nesting_level++;
            consume();
            break;
        case '}':
        case ']':
            nesting_level--;
            if (nesting_level < 0)
                return 0;
            consume();
            break;
        default:
            consume();
            break;
        }
    }

    return 1;
}

int skip_value() {
    skip_blank_characters();
    char ch = peek();
    if (ch != ':')
        return 0;
    consume();

    skip_blank_characters();
    ch = peek();

    if (ch != '{' && ch != '[' && ch != '"') {
        int i = 0;
        char str_value[10];
        while ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
            ch = consume();
            str_value[i++] = ch;
            ch = peek();
        }
        str_value[i] = '\0';

        return are_equal_strings(str_value, "null") ||
               are_equal_strings(str_value, "false") ||
               are_equal_strings(str_value, "true") ||
               is_number_string(str_value);
    }
    if (ch == '"') {
        return skip_json_string();
    }

    char nesting_chain[64];
    for (int i = 0; i < 64; ++i)
        nesting_chain[i] = '\0';
    char nesting_chain_index = 0;
    nesting_chain[nesting_chain_index++] = ch;
    consume();

    while (nesting_chain[0] != '\0' && ch != EOF) {
        ch = peek();
        switch (ch) {
        case '"':
            skip_json_string();
            if (!skip_value())
                return 0;
            break;
        case '[':
        case '{':
            nesting_chain[nesting_chain_index++] = ch;
            consume();
            break;
        case '}':
        case ']':
            nesting_chain_index--;
            if ((ch == ']' && nesting_chain[nesting_chain_index] == '[') ||
                (ch == '}' && nesting_chain[nesting_chain_index] == '{')) {
                nesting_chain[nesting_chain_index] = '\0';
                consume();
            } else {
                return 0;
            }
            break;
        default:
            consume();
            break;
        }
    }

    if (ch == EOF && nesting_chain[0] != '\0') {
        return 0;
    }

    return 1;
}

int get_json_string_value(char *out) {
    skip_blank_characters();
    char ch = peek();
    if (ch != '"')
        return 0;
    consume();
    ch = peek();

    if (ch == '"') {
        out[0] = '\0';
        consume();
        return 1;
    }

    while (ch != '"') {
        ch = peek();
        switch (ch) {
        case '"':
            consume();
            break;
        case EOF:
            return 0;
        default:
            ch = consume();
            *out++ = ch;
            break;
        }
    }
    *out = '\0';
    return 1;
}

int skip_json_string() {
    skip_blank_characters();
    char ch = peek();
    if (ch != '"')
        return 0;
    consume();
    ch = peek();

    if (ch == '"') {
        consume();
        return 1;
    }

    while (ch != '"') {
        ch = peek();
        switch (ch) {
        case '"':
            consume();
            break;
        case EOF:
            return 0;
        default:
            ch = consume();
            break;
        }
    }

    return 1;
}

void skip_blank_characters() {
    char ch;
    while ((ch = peek()) == '\n' || ch == '\t' || ch == ' ')
        consume();
}

char peek() {
    const char ch = (char)fgetc(file);
    fseek(file, -1, SEEK_CUR);
    return ch;
};

char consume() {
    const char ch = (char)fgetc(file);
    return ch;
};
