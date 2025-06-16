#include <string.h>

char *strtok_r(char *str, const char *del, char **saveptr)
{
    if (str != 0)
        *saveptr = str;
    char *s = *saveptr;
    int i = 0;
    int exs = 0;
    char *b = 0;
    char *e = 0;
    while (s[i])
    {
        int j = 0;
        while (del[j])
        {
            if (del[j] == s[i])
                break;
            j++;
        }
        if (del[j])
        {
            if (exs)
            {
                e = s + i;
                break;
            }
        }
        else
        {
            if (exs == 0)
                b = s + i;
            exs = 1;
        }
        i++;
    }
    *saveptr = (e != 0) ? (e + 1) : (s + i);
    if (exs == 0)
        return 0;
    if (s[i] != 0)
        *e = 0;
    return b;
}

