#include "main.h"

/**
 * gethistory - gets the history list
 * Return: 0 uposon success
 */

State **gethistory()
{
    static State *hlist;

    return (&hlist);
}

/**
 * sethist - set hist and value
 * @cmd: command
 * Return: 0 upon success
 */

int sethist(char *cmd)
{
    State **hlistroot = gethistory();
    State *hlist = *hlistroot;
    State *ptr = hlist, *new;

    if (hlist == NULL)
    {
        new = malloc(sizeof(State));
        if (new == NULL)
            return (-1);

        new->cmd = _strdup(cmd);
        new->next = NULL;
        *hlistroot = new;
        return (0);
    }
    while (ptr->next != NULL)
        ptr = ptr->next;

    new = malloc(sizeof(State));
    if (new == NULL)
        return (-1);
    new->cmd = _strdup(cmd);
    new->next = NULL;
    ptr->next = new;
    return (0);
}

/**
 * print_hist - prints all elements of listint
 *
 * Return: num of elements
 */

int print_hist(void)
{
    State **hlistroot = gethistory();
    State *h = *hlistroot;
    int i;
    int len, numlen;
    char *s, *num;

    i = 0;
    while (h != NULL)
    {
        len = _strlen(h->cmd);
        s = h->cmd;
        num = itos(i);
        numlen = _strlen(num);
        write(1, num, numlen);
        _putchar(' ');
        write(1, s, len);
        h = h->next;
        i++;
    }
    return (i);
}

/**
 * exit_hist - exit history and copy to file
 * Return: int
 */

int exit_hist(void)
{

    int fd;
    char *file = ".shell_history";
    int len;
    char *s;

    State **hlistroot = gethistory();
    State *hlist = *hlistroot;
    State *ptr = hlist;

    fd = open(file, O_CREAT | O_RDWR, 0600);
    if (fd == -1)
        return (-1);

    while (hlist != NULL)
    {
        ptr = hlist->next;
        s = hlist->cmd;
        len = _strlen(s);
        write(fd, s, len);
        free(hlist->cmd);
        free(hlist);
        hlist = ptr;
    }

    close(fd);
    return (1);
}
