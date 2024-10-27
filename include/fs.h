#ifndef FS_H
#define FS_H

int save_record(struct account_record record);
int search_records(int (*search_algo)(char *line, char *search_str), char *str,
                   int size, struct account_record *records);

#endif
