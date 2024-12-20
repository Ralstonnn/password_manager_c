#include "db.h"
#include "common.h"
#include "encryption.h"
#include "paths.h"
#include <sqlite3.h>
#include <stdio.h>

int handle_set_record_from_db(void *data, int argc, char **argv,
                              char **azColName);
int handle_search_records(void *data, int argc, char **argv, char **azColName);
int handle_get_records_count(void *data, int argc, char **argv,
                             char **azColName);

static int SQL_RECORDS_ITERATOR = 0;

int init_db() {
    FILE *file;
    file = fopen(DB_FILE_PATH, "r");

    if (file != NULL) {
        fclose(file);
        return 1;
    }

    sqlite3 *db;
    int rc;

    rc = sqlite3_open(DB_FILE_PATH, &db);
    rc = sqlite3_exec(db, DB_MIGRATION, NULL, NULL, NULL);

    return !rc;
}

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

int save_record_db(struct account_record *record) {
    sqlite3 *db;

    if (sqlite3_open(DB_FILE_PATH, &db)) {
        return 0;
    }

    char sql[1024];
    unsigned char password_encrypted[500];

    if (!encrypt_str((unsigned char *)record->password, password_encrypted)) {
        return 1;
    }

    sprintf(sql,
            "INSERT INTO records (name, username, password) VALUES (\"%s\", "
            "\"%s\", \"%s\")",
            record->name, record->username, password_encrypted);

    if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
        return 1;
    }

    record->id = sqlite3_last_insert_rowid(db);
    sqlite3_close(db);
    return 0;
}

int update_record_db(struct account_record record) {
    char sql[1024];
    unsigned char password_encrypted[500];

    if (!encrypt_str((unsigned char *)record.password, password_encrypted)) {
        return 1;
    }

    sprintf(sql,
            "UPDATE records SET name=\"%s\", username=\"%s\", password=\"%s\" "
            "WHERE id = %i",
            record.name, record.username, (char *)password_encrypted,
            record.id);

    return request_db(sql, NULL, NULL);
}

int delete_record_db(int id) {
    char sql[100];
    sprintf(sql, "DELETE FROM records WHERE id=%i", id);
    return request_db(sql, NULL, NULL);
}

int search_records_db(struct account_record *records, char *search_str,
                      const int max_size, const int page) {
    char sql[512];
    sprintf(sql,
            "SELECT id, name, username FROM records WHERE name LIKE "
            "\"%%%s%%\" LIMIT %i,%i",
            search_str, page * max_size, max_size);
    return request_db(sql, records, handle_search_records);
}

int get_record_by_id_db(struct account_record *record, const int id) {
    char sql[256];
    sprintf(sql,
            "SELECT id, name, username, password FROM records WHERE id = %i",
            id);
    return request_db(sql, record, handle_set_record_from_db);
}

int get_record_with_offset_db(struct account_record *record, const int limit,
                              const int offset) {
    char sql[100];
    sprintf(
        sql,
        "SELECT id, name, username, password FROM records LIMIT %i OFFSET %i",
        limit, offset);
    return request_db(sql, record, handle_set_record_from_db);
}

int get_records_count_db() {
    const char sql[30] = "SELECT count(id) FROM records";
    int count = 0;
    if (request_db(sql, &count, handle_get_records_count))
        return 0;
    return count;
}

int get_search_by_name_records_count_db(char *name) {
    char sql[256];
    sprintf(sql, "SELECT count(id) FROM records WHERE name LIKE \"%%%s%%\"",
            name);
    int count = 0;
    if (request_db(sql, &count, handle_get_records_count))
        return 0;
    return count;
}

int handle_search_records(void *data, int argc, char **argv, char **azColName) {
    struct account_record *record =
        (struct account_record *)data + SQL_RECORDS_ITERATOR;

    handle_set_record_from_db(record, argc, argv, azColName);

    SQL_RECORDS_ITERATOR++;
    return 0;
}

int handle_set_record_from_db(void *data, int argc, char **argv,
                              char **azColName) {
    struct account_record *record = (struct account_record *)data;

    for (int i = 0; i < argc; i++) {
        char *value = argv[i] ? argv[i] : "NULL";
        char *colName = azColName[i];

        if (are_equal_strings(colName, "id")) {
            record->id = string_to_int(value);
        } else if (are_equal_strings(colName, "name")) {
            copy_str(value, record->name);
        } else if (are_equal_strings(colName, "username")) {
            copy_str(value, record->username);
        } else if (are_equal_strings(colName, "password")) {
            char password_decrypted[500];
            if (!decrypt_str((const char *)value,
                             (unsigned char *)password_decrypted)) {
                return 1;
            }
            copy_str(password_decrypted, record->password);
        }
    }

    return 0;
}

int handle_get_records_count(void *data, int argc, char **argv,
                             char **azColName) {
    for (int i = 0; i < argc; i++) {
        char *value = argv[i] ? argv[i] : "NULL";
        char *colName = azColName[i];

        if (are_equal_strings(colName, "count(id)")) {
            *(int *)data = string_to_int(value);
        }
    }
    return 0;
}
