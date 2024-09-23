#include <regex.h>
#include <stdio.h>

int validate_with_regexp(char *str, char *pattern)
{
    regex_t regex;
    int result;

    result = regcomp(&regex, pattern, REG_EXTENDED);

    if (result)
        return 0;

    result = regexec(&regex, str, 0, NULL, 0);

    if (result == 0)
        result = 1;
    else
        result = 0;

    regfree(&regex);
    return result;
}

int validate_password(char *str)
{
    char pattern[] = "^[A-Za-z0-9!@#$%&*^]+$";
    return validate_with_regexp(str, pattern);
}

int is_number_string(char *str)
{
    char pattern[] = "^[0-9]+$";
    return validate_with_regexp(str, pattern);
}
