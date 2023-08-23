#include "main.h"
int scriptmode(char *av[]);
/**
 * main - runs a shell
 *
 * @ac: number of args
 * @av: command line arg matrix
 * @environ: environment matrix
 *
 * Return: return value of last command
 */
int main(int ac, char *av[], char **environ)
{
    int ret = 0;

    char *pidptr;

    initsvars(ac - 1, av);
    pidptr = _getpid();
    setsvar("$", pidptr);
    free(pidptr);
    _getline(NULL, -2);

    setallenv(environ, NULL);
    if (ac > 1)
        ret = scriptmode(av);
    else
        ret = shintmode();
    exitcleanup(NULL);
    exit_hist();
    return (ret);
}
