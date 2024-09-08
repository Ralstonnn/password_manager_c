#ifndef FS_H
#define FS_H

int save_record(struct account_record record);
int search_records(char *str, int size, struct account_record *records);

#endif