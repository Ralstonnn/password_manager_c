#ifndef COMMON_H
#define COMMON_H

#define MAX_BUFFER_SIZE 256
#define USERNAME_BUFFER_SIZE 56

struct account_record {
    char name[USERNAME_BUFFER_SIZE];
    char username[USERNAME_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
};

#endif