#include "db.h"
#include "common.h"
#include <sqlite3.h>
#include <stdio.h>

int handle_search_records(void *data, int argc, char **argv, char **azColName);

static int SQL_RECORDS_ITERATOR = 0;

int request_db(char *sql, void *data,
               int(callback)(void *data, int argc, char **argv,
                             char **azColName)) {
    int result = 0;
    sqlite3 *db;
    char *errMsg = 0;
    int rc;

    rc = sqlite3_open(DB_FILE_PATH, &db);

    if (rc) {
        printf("Can't open database\n");
    }

    rc = sqlite3_exec(db, sql, callback, data, &errMsg);
    SQL_RECORDS_ITERATOR = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        result = 1;
    }

    sqlite3_close(db);
    return result;
}

int save_record_db(struct account_record record) {
    char sql[500];
    sprintf(sql,
            "INSERT INTO records (name, username, password) VALUES (\"%s\", "
            "\"%s\", \"%s\")",
            record.name, record.username, record.password);
    return request_db(sql, NULL, NULL);
}

void search_records_db(struct account_record *records, char *search_str,
                       int max_size) {
    char sql[256];
    sprintf(sql,
            "SELECT name, username, password FROM records WHERE name LIKE "
            "\"%%%s%%\" LIMIT %i",
            search_str, max_size);
    request_db(sql, records, handle_search_records);
}

int handle_search_records(void *data, int argc, char **argv, char **azColName) {
    int i;
    struct account_record *record =
        (struct account_record *)data + SQL_RECORDS_ITERATOR;

    for (i = 0; i < argc; i++) {
        char *value = argv[i] ? argv[i] : "NULL";
        char *colName = azColName[i];

        if (are_equal_strings(colName, "name"))
            copy_str(value, record->name);
        else if (are_equal_strings(colName, "username"))
            copy_str(value, record->username);
        else if (are_equal_strings(colName, "password"))
            copy_str(value, record->password);
    }

    SQL_RECORDS_ITERATOR++;
    return 0;
}
