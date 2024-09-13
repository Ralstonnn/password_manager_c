#include "common.h"

int string_start_match(char *line, char *search_str)
{
    while (!is_end_of_string_char(*search_str))
        if (*search_str++ != *line++)
            return 0;
    if (*search_str != *line)
        return 0;

    return 1;
}

int string_fuzzy_find(char *line, char *search_str)
{
    int search_srt_len = get_str_length(search_str);
    int line_len = get_str_length(line);
    int i, j;
    i = j = 0;

    for (; i < search_srt_len; i++)
    {
        for (; j < line_len; j++)
        {
            if (search_str[i] == line[j])
            {
                j++;
                break;
            }
            else if (j == line_len - 1)
            {
                return 0;
            }
        }
    }

    return 1;
}
