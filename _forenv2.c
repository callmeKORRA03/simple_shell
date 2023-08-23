#include "main.h"
/**
 * _putchar - writes the character c to stdout
 * @c: The character to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int _putchar(char c)
{
    return (write(1, &c, 1));
}
/**
 * _forenv - print environment
 * Return: 0 if success
 */
int _forenv(void)
{
    char **environ = *(getenviron());
    int i;
    int j;

    i = 0;
    while (environ[i])
    {
        j = 0;
        while (environ[i][j] != 0)
        {
            _putchar(environ[i][j]);
            j++;
        }
        _putchar('\n');
        i++;
    }
    return (0);
}

/**
 * getalist - static wrapper getalias list
 * Return: address alist
 *
 */
alias **getalist()
{
    static alias *alist;

    return (&alist);
}
/**
 * getalias - get alias value
 * returns original argument if not found
 * @name: name of alias
 * Return: original argument if not found, otherwise value
 */
char *getalias(char *name)
{
    alias *alist = *(getalist());
    alias *ptr = alist;

    while (ptr != NULL && _strcmp(ptr->name, name))
    {
        ptr = ptr->next;
    }
    if (ptr == NULL)
    {
        return (name);
    }
    free(name);
    return (getalias(_strdup(ptr->val)));
}
/**
 * setalias - set alias and value
 * @name: name of alias
 * @val: value to set alias to
 * Return: 0 upon success
 */
int setalias(char *name, char *val)
{
    alias **alistroot = getalist();
    alias *alist = *alistroot;
    alias *ptr = alist, *new;

    if (alist == NULL)
    {
        new = malloc(sizeof(alias));
        if (new == NULL)
            return (-1);
        new->name = name;
        new->val = val;
        new->next = NULL;
        *alistroot = new;
        return (0);
    }
    while (_strcmp(ptr->name, name) && ptr->next != NULL)
        ptr = ptr->next;
    if (!_strcmp(ptr->name, name))
    {
        free(ptr->val);
        ptr->val = val;
        free(name);
    }
    else
    {
        new = malloc(sizeof(alias));
        if (new == NULL)
            return (-1);
        new->name = name;
        new->val = val;
        new->next = NULL;
        ptr->next = new;
    }
    return (0);
}
/**
 * unsetalias - unset alias of @name
 * @name: name of alias value to unset
 * Return: 0 if sucess
 */
int unsetalias(char *name)
{
    alias *alist = *(getalist());
    alias *ptr = alist, *next;

    if (alist == NULL)
        return (0);
    if (!(_strcmp(ptr->name, name)))
    {
        alist = alist->next;
        free(ptr->val);
        free(ptr);
        return (0);
    }

    while (ptr->next != NULL && _strcmp(ptr->next->name, name))
        ptr = ptr->next;
    if (!_strcmp(ptr->next->name, name))
    {
        next = ptr->next->next;
        free(ptr->next->name);
        free(ptr->next);
        ptr->next = next;
    }
    return (0);
}
/**
 * aliascmd - tokenizes arguments for alias command
 * @av: arguments
 * Return: 1
 */
int aliascmd(char *av[])
{
    alias *alist = *(getalist());
    alias *ptr = alist;
    int i;
    char *name, *val;

    if (av[1] == NULL)
    {
        while (ptr != NULL)
        {
            fprintstrs(1, ptr->name, "='", ptr->val, "'\n", NULL);
            ptr = ptr->next;
        }
        return (0);
    }
    for (i = 1; av[i] != NULL; i++)
    {
        name = _tokeniser(av[i], "=");
        val = _tokeniser(NULL, "=");
        if (val != NULL)
        {
            name = _strdup(name);
            if (name == NULL)
                return (-1);
            val = _strdup(val);
            if (val == NULL)
            {
                free(name);
                return (-1);
            }
            setalias(name, val);
        }
        else
        {
            val = _strdup(name);
            val = getalias(val);
            if (!_strcmp(val, name))
            {
                fprintstrs(1, "alias: ", name, " not found\n", NULL);
                free(val);
            }
            else
            {
                fprintstrs(1, name, "='", val, "'\n", NULL);
                free(val);
            }
        }
    }
    return (0);
}
