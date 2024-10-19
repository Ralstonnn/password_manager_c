#ifndef DB_H
#define DB_H

#include "common.h"

#define DB_FILE_PATH "../test.db"

int request_db(char *sql, void *data, int(callback)(void *data, int argc, char **argv, char **azColName));
void search_records_db(struct account_record *records, char *search_str, int max_size);

#endif
