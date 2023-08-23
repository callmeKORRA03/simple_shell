#include "main.h"
/**
 * parsesetsvar - parse set shell vars, returns new buf after var setting
 * @buf: buffer
 * Return: string
 */
char *parsesetsvar(char *buf)
{
    int haseq;
    char *ptr, *name, *val, *bufstart = buf, *newbuf;

    do
    {
        haseq = 0;
        for (ptr = buf; *ptr; ptr++)
        {
            if (*ptr == '=')
            {
                name = _tokeniser(buf, " ");
                buf = _tokeniser(NULL, "");
                ptr = buf;
                haseq = 1;
                name = _tokeniser(name, "=");
                val = _tokeniser(NULL, "");
                setsvar(name, val);
                if (buf == NULL)
                {
                    free(bufstart);
                    return (NULL);
                }
                continue;
            }
            if (*ptr == ' ' || *ptr == 0)
            {
                buf = _strdup(buf);
                free(bufstart);
                return (buf);
            }
            if (ptr == NULL)
            {
                free(bufstart);
                return (NULL);
            }
        }
    } while (haseq && *ptr != 0);
    newbuf = malloc(sizeof(char) * (_strlen(buf) + 1));
    if (newbuf == NULL)
        return (NULL);
    newbuf = forcopy(newbuf, buf);
    free(bufstart);
    return (newbuf);
}

char *subsvars(char **buf)
{
    char *varptr = *buf, *ptr, *name, *val, *valptr, *dest, *dolptr;
    size_t buflen = _strlen(*buf);
    size_t varvlen, varnlen, i;
    int inquotes = 0;

    while (*varptr != 0)
    {
        while (*varptr != '$' && *varptr != 0)
        {
            if (*varptr == '\\')
            {
                varptr++;
                if (*varptr != 0)
                    varptr++;
                continue;
            }
            if (inquotes == 2 && *varptr == '"')
                inquotes = 0;
            if (inquotes == 0 && *varptr == '"')
                inquotes = 2;
            if (*varptr == '\'' && inquotes != 2)
            {
                varptr++;
                while (*varptr != '\'' && *varptr != 0)
                    varptr++;
            }
            varptr++;
            if (*varptr == '$' &&
                (varptr[1] == ' ' || varptr[1] == 0 || varptr[1] == '\n'))
                varptr++;
        }
        dolptr = varptr;
        if (*varptr == 0)
            return (*buf);
        varptr++;
        for (ptr = varptr, varnlen = 0; *ptr != 0 && *ptr != ' ' && *ptr != '\n' && *ptr != '\\'; ptr++)
            varnlen++;
        name = malloc(sizeof(char) * (varnlen + 1));
        if (name == NULL)
            return (NULL);
        for (i = 0; i < varnlen; i++, varptr++)
            name[i] = *varptr;
        name[i] = 0;
        val = _getenv(name);
        if (val == name)
        {
            val = getsvar(name);
            if (val == name)
                val = _strdup("");
        }
        free(name);
        varvlen = _strlen(val);
        buflen = buflen - varnlen + varvlen + 1;
        name = malloc(sizeof(char) * (buflen));
        for (ptr = *buf, dest = name, valptr = val; *ptr != 0; ptr++, dest++)
        {
            if (val != NULL && ptr == dolptr)
            {
                while (*valptr != 0)
                    *dest++ = *valptr++;
                free(val);
                val = NULL;
                varptr = dest;
                ptr += varnlen + 1;
                if (*ptr == 0)
                    break;
            }
            *dest = *ptr;
        }
        *dest = *ptr;
        free(*buf);
        *buf = name;
    }
    return (*buf);
}

/**
 * cleanarg - cleans escapes and functional quotes
 *
 * @arg: - argument to clean
 *
 * Return: cleaned argument
 */
char *cleanarg(char *arg)
{
    char *newbuf, *ptr, *ptr2;
    size_t len = 0;
    int inquote = 0;

    ptr = arg;
    while (*ptr != 0)
    {
        if (*ptr == '\\' && !inquote)
        {
            ptr++;
            if (*ptr != 0)
            {
                len++;
                ptr++;
            }
            continue;
        }
        if (*ptr == '\\' && inquote == 2)
        {
            ptr++;
            if (*ptr == '$' || *ptr == '#' || *ptr == ';' || *ptr == '\\')
            {
                len++;
                ptr++;
            }
            else
                len++;
            continue;
        }
        if (!inquote && *ptr == '"')
        {
            inquote = 2;
            ptr++;
            continue;
        }
        if (!inquote && *ptr == '\'')
        {
            inquote = 1;
            ptr++;
            continue;
        }
        if ((inquote == 1 && *ptr == '\'') || (inquote == 2 && *ptr == '"'))
        {
            inquote = 0;
            ptr++;
            continue;
        }
        if (*ptr == 0)
            break;
        ptr++;
        len++;
    }
    newbuf = malloc(sizeof(char) * (len + 1));
    if (newbuf == NULL)
        return (NULL);
    ptr = arg;
    ptr2 = newbuf;
    inquote = 0;
    while (*ptr != 0)
    {
        if (*ptr == '\\' && !inquote)
        {
            ptr++;
            if (*ptr != 0)
                *ptr2++ = *ptr++;
            continue;
        }
        if (*ptr == '\\' && inquote == 2)
        {
            ptr++;
            if (*ptr == '$' || *ptr == '#' || *ptr == ';' || *ptr == '\\')
                *ptr2++ = *ptr++;
            else
                *ptr2++ = '\\';
            continue;
        }
        if (!inquote && *ptr == '"')
        {
            inquote = 2;
            ptr++;
            continue;
        }
        if (!inquote && *ptr == '\'')
        {
            inquote = 1;
            ptr++;
            continue;
        }
        if ((inquote == 1 && *ptr == '\'') || (inquote == 2 && *ptr == '"'))
        {
            inquote = 0;
            ptr++;
            continue;
        }
        if (*ptr != 0)
            *ptr2++ = *ptr++;
    }
    *ptr2 = 0;
    free(arg);
    return (newbuf);
}

/**
 * tildeexpand - handle expanding ~ where appropriate
 *
 * @buf: buffer to process
 *
 * Return: processed buffer
 */
char *tildeexpand(char *buf)
{
    char *tildeptr = buf, *endptr, *homepath, *newbuf, *bufptr, *newptr;
    int inquotes = 0;

    while (*tildeptr != 0)
    {
        tildeptr = buf;
        while (*tildeptr != '~' && *tildeptr != 0)
        {
            if (*tildeptr == '\\')
            {
                tildeptr++;
                if (*tildeptr != 0)
                    tildeptr++;
                continue;
            }
            if (inquotes != 1 && *tildeptr == '"')
            {
                inquotes = 2;
                while (*tildeptr != '"' && *tildeptr != 0)
                {
                    if (*tildeptr == '\\')
                    {
                        tildeptr++;
                        inquotes = *tildeptr != 0 && tildeptr++;
                        inquotes = 2;
                        continue;
                    }
                    tildeptr++;
                }
            }
            if (*tildeptr == '\'' && inquotes != 2)
            {
                tildeptr++;
                while (*tildeptr != '\'' && *tildeptr != 0)
                    tildeptr++;
            }
            tildeptr++;
        }
        if (*tildeptr == 0)
            return (buf);
        endptr = tildeptr;
        while (*endptr != '/' && *endptr != ' ' && *endptr != 0)
            endptr++;
        homepath = _getenv("HOME");
        if (homepath == NULL)
            return (NULL);
        newbuf = malloc(_strlen(buf) - (size_t)endptr +
                        (size_t)tildeptr + _strlen(homepath) + 1);
        if (newbuf == NULL)
        {
            free(homepath);
            return (NULL);
        }
        bufptr = buf;
        newptr = newbuf;
        while (bufptr < tildeptr)
            *newptr++ = *bufptr++;
        bufptr = homepath;
        while (*bufptr)
            *newptr++ = *bufptr++;
        while (*endptr)
            *newptr++ = *endptr++;
        *newptr = 0;
        free(homepath);
        free(buf);
        buf = newbuf;
    }
    return (newbuf);
}
/**
 * parseargs - parse arguments function, frees buf at end
 * @buf: buffer pointer
 * Return: return value of command
 */
int parseargs(char **buf)
{
    char *av[1024], *ptr, *left, *right;
    int ac, ret = 0, newchk;

    if (*buf == NULL || **buf == 0)
        return (0);
    ptr = *buf;
    newchk = _strlen(*buf) - 1;
    if (ptr[newchk] == '\n')
        ptr[newchk] = 0;
    if (*buf[0] == 0)
    {
        free(*buf);
        return (0);
    }
    left = _strdup(strtokqe(*buf, ";", 7));
    right = _strdup(strtokqe(NULL, "", 7));
    free(*buf);
    *buf = left;
    if (right != NULL && *right != 0)
    {
        parseargs(&left);
        return (parseargs(&right));
    }
    left = strtokqe(*buf, "&", 7);
    right = strtokqe(NULL, "", 7);
    if (right != NULL && *right == '&')
    {
        left = _strdup(left);
        right = _strdup(right);
        free(*buf);
        *buf = left;
        ret = parseargs(&left);
        *buf = right;
        right++;
        right = _strdup(right);
        free(*buf);
        if (ret == 0)
            return (parseargs(&right));
        *buf = right;
        strtokqe(right, "|", 7);
        right = strtokqe(NULL, "", 7);
        if (right != NULL)
        {
            right++;
            right = _strdup(right);
            free(*buf);
            return (parseargs(&right));
        }
        free(*buf);
        return (ret);
    }
    else if (right != NULL)
    {
        *(right - 1) = '&';
    }
    left = strtokqe(*buf, "|", 7);
    right = strtokqe(NULL, "", 7);
    if (right != NULL && *right == '|')
    {
        left = _strdup(left);
        right = _strdup(right);
        free(*buf);
        *buf = left;
        ret = parseargs(&left);
        *buf = right;
        right++;
        right = _strdup(right);
        free(*buf);
        if (ret != 0)
            return (parseargs(&right));
        free(right);
        return (ret);
    }
    else if (right != NULL)
    {
        *(right - 1) = '|';
    }
    *buf = subsvars(buf);
    if (*buf == NULL)
        return (-1);
    *buf = tildeexpand(*buf);
    if (*buf == NULL)
        return (-1);
    *buf = parsesetsvar(*buf);
    if (*buf == NULL)
        return (0);
    ac = 0;
    av[ac++] = _strdup(strtokqe(*buf, "\n ", 7));
    av[0] = getalias(av[0]);
    if (av[0] != NULL)
        av[0] = cleanarg(av[0]);
    while (av[ac - 1] != NULL)
    {
        av[ac] = _strdup(strtokqe(NULL, "\n ", 7));
        if (av[ac] != NULL)
            av[ac] = cleanarg(av[ac]);
        ac++;
    }
    ac--;
    av[ac] = NULL;
    free(*buf);
    *buf = NULL;
    ret = builtincall(av);
    for (ac = 0; av[ac] != NULL; ac++)
        free(av[ac]);
    return (ret);
}
