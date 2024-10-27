#ifndef DB_H
#define DB_H

#include "common.h"

#define DB_FILE_PATH "../test.db"

int request_db(char *sql, void *data,
               int(callback)(void *data, int argc, char **argv,
                             char **azColName));
int search_records_db(struct account_record *records, char *search_str,
                      int max_size);
int save_record_db(struct account_record record);
int get_record_by_id_db(struct account_record *record, int id);

#endif
