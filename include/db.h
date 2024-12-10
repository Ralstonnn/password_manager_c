#ifndef DB_H
#define DB_H

#include "common.h"

int init_db();
int request_db(char *sql, void *data,
               int(callback)(void *data, int argc, char **argv,
                             char **azColName));
int search_records_db(struct account_record *records, char *search_str,
                      int max_size, int page);
int save_record_db(struct account_record *record);
int update_record_db(struct account_record record);
int delete_record_db(int id);
int get_record_by_id_db(struct account_record *record, int id);
int get_record_with_offset_db(struct account_record *record, const int limit,
                              const int offset);
int get_records_count_db();
int get_search_by_name_records_count_db(char *name);

#define DB_MIGRATION                                                           \
    "CREATE TABLE records ("                                                   \
    "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"                           \
    "name TEXT NOT NULL,"                                                      \
    "username TEXT NOT NULL,"                                                  \
    "password TEXT NOT NULL"                                                   \
    ");"
#endif
