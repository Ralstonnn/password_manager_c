#ifndef DB_H
#define DB_H

#include "common.h"

#define DB_FILE_PATH "../test.db"

int init_db();
int request_db(char *sql, void *data,
               int(callback)(void *data, int argc, char **argv,
                             char **azColName));
int search_records_db(struct account_record *records, char *search_str,
                      int max_size);
int save_record_db(struct account_record *record);
int delete_record_db(int id);
int get_record_by_id_db(struct account_record *record, int id);

#define DB_MIGRATION                                                           \
    "CREATE TABLE records ("                                                   \
    "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"                           \
    "name TEXT NOT NULL,"                                                      \
    "username TEXT NOT NULL,"                                                  \
    "password TEXT NOT NULL"                                                   \
    ");"
#endif
