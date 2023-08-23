#include "main.h"
/**
 * _getline - gets a line from fd or std input
 * @lineptr: buffer to fill line with
 * @fd: file descriptor
 * Return: num of characters
 */
int _getline(char **lineptr, int fd)
{
    int size = 1025;
    int old_size = 0;
    int r = 1;
    int sum = 0;
    static char buffer[1025];
    static int begin;
    static int end;
    int c = 0;
    int d;

    if (fd == -2)
    {
        begin = 0;
        end = 0;
    }

    if (lineptr == NULL)
    {
        return (0);
    }
    if (*lineptr == NULL)
    {
        *lineptr = malloc(sizeof(char) * size + 1);
        if (*lineptr == NULL)
            return (-1);
    }

    while (1)
    {
        if (begin == end)
        {
            while (sum < 1024 && r != 0)
            {
                r = read(fd, buffer + sum, 1024 - sum);
                begin = 0;
                sum += r;
                end = sum;
                for (d = 0; r != 0 && d < end; d++)
                    if (buffer[d] == '\n')
                        r = 0;
            }
            if (sum == 0)
            {
                buffer[0] = 0;
                return (sum);
            }
            buffer[sum] = 0;
            sum = 0;
        }
        for (; buffer[begin]; begin++)
        {
            if (begin == 1024)
            {
                break;
            }
            if (buffer[begin] == '\n')
            {
                (*lineptr)[c] = '\n';
                begin++;
                c++;
                (*lineptr)[c] = '\0';
                return (c);
            }
            else
            {
                (*lineptr)[c] = buffer[begin];
            }
            c++;
        }
        if (c + begin >= 1024)
        {
            old_size = size;
            size = size + 1024;
            *lineptr = _realloc(*lineptr, old_size, size);
            if (*lineptr == NULL)
            {
                return (-1);
            }
        }
        else
        {
            (*lineptr)[old_size + begin] = 0;
            return (c);
        }
    }
}

/**
 * _tokeniser - tokenizes a string
 * @str: string to tokenize
 *
 * @delim: delimiters used to create tokens
 *
 * Return: token
 */
char *_tokeniser(char *str, char *delim)
{
    static char *saved_string;
    int i;
    int j;
    char *tmp_str;
    char *tmp_delim;

    if (str == 0)
        str = saved_string;
    if (str == 0)
        return (0);

    tmp_str = str;
    tmp_delim = delim;
    i = 0;
    while (tmp_str[i] != 0)
    {
        j = 0;
        while (delim[j] != 0)
        {
            if (tmp_str[i] == tmp_delim[j])
                break;
            j++;
        }
        if (tmp_delim[j] == 0)
            break;
        i++;
    }
    str = str + i;
    if (*str == 0)
    {
        saved_string = str;
        return (0);
    }
    tmp_str = tmp_str + i;

    i = 0;
    while (tmp_str[i] != 0)
    {
        j = 0;
        while (tmp_delim[j] != 0)
        {
            if (tmp_str[i] == tmp_delim[j])
                break;
            j++;
        }
        if (tmp_delim[j] != 0)
            break;
        i++;
    }
    saved_string = tmp_str;
    if (tmp_str[i] != 0)
    {
        saved_string = (saved_string + i + 1);
        tmp_str[i] = '\0';
    }
    else
    {
        saved_string = '\0';
    }
    return (tmp_str);
}
/**
 * strtokqe - string token with quotes and escapes
 * @str: string
 * @delim: delimiters
 * @escflags: escape flags
 * flags are bitwise.
 * 1 = use \ to escape delims
 * 2 = single quote skips
 * 4 = double quote skips
 *
 * Return: string
 */
char *strtokqe(char *str, char *delim, int escflags)
{
    static char *saved_string;
    int i;
    int j;

    if (str == 0)
        str = saved_string;
    if (str == 0)
        return (0);

    i = 0;
    while (str[i] != 0)
    {
        j = 0;
        while (delim[j] != 0)
        {
            if (str[i] == delim[j])
                break;
            j++;
        }
        if (delim[j] == 0)
            break;
        i++;
    }
    str = str + i;
    if (*str == 0)
    {
        saved_string = str;
        return (0);
    }

    i = 0;
    while (str[i] != 0)
    {
        if (str[i] == '\\' && escflags & 1)
        {
            if (str[i + 1] != 0)
                i++;
            i++;
            continue;
        }
        if (str[i] == '\'' && escflags & 2)
        {
            i++;
            while (str[i] != '\'')
            {
                if (str[i] == '\\' && escflags & 1)
                {
                    if (str[i + 1] != 0)
                        i++;
                    i++;
                    continue;
                }
                i++;
            }
        }
        if (str[i] == '"' && escflags & 4)
        {
            i++;
            while (str[i] != '"')
            {
                if (str[i] == '\\' && escflags & 1)
                {
                    if (str[i + 1] != 0)
                        i++;
                    i++;
                    continue;
                }
                i++;
            }
        }
        j = 0;
        while (delim[j] != 0)
        {
            if (str[i] == delim[j])
                break;
            j++;
        }
        if (delim[j] != 0)
            break;
        i++;
    }
    saved_string = str;
    if (str[i] != 0)
    {
        saved_string = (saved_string + i + 1);
        str[i] = '\0';
    }
    else
    {
        saved_string = '\0';
    }
    return (str);
}

/**
 * _realloc - reallocates a memory block using malloc and free
 * @ptr: void pointer
 * @old_size: unsigned int
 * @new_size: unsigned int
 * Return: pointer
 */
void *_realloc(void *ptr, unsigned int old_size, unsigned int new_size)
{
    void *ret = 0;
    char *write, *read;

    if (new_size == old_size)
        return (ptr);

    if (new_size > 0 || ptr == 0)
    {
        ret = malloc(new_size);
        if (ret == 0)
            return (0);
    }
    if (new_size > 0 && ptr != 0)
    {
        write = ret;
        read = ptr;
        if (new_size < old_size)
            old_size = new_size;
        while (old_size)
        {
            old_size--;
            *write++ = *read++;
        }
    }

    free(ptr);
    return (ret);
}
