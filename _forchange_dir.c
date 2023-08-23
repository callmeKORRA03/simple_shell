#include "main.h"
/**
 *_cd - change directory builtin
 * @av: argument to work with
 * Return: int
 */
int _cd(char *av[])
{
    char *oldpwd = NULL, *newpath, *pathbit, *newptr;
    char *homestr = "HOME", *oldpwdstr = "OLDPWD";
    int ret, printpath = 0;

    oldpwd = getcwd(oldpwd, 0);
    if (oldpwd == NULL)
        return (1);
    if (av[1] == NULL || av[1][0] == 0)
    {
        newpath = _getenv(homestr);
        if (newpath == homestr)
            newpath = _strdup("");
        av[1] = newpath;
        av[2] = NULL;
    }
    else if (av[1][0] == '-' && av[1][1] == 0)
    {
        newpath = _getenv(oldpwdstr);
        if (newpath == oldpwdstr)
            newpath = _strdup("");
        printpath = 1;
        free(av[1]);
        av[1] = newpath;
    }
    newpath = malloc(sizeof(char) * (_strlen(oldpwd) + _strlen(av[1]) + 2));
    if (newpath == NULL)
        return (-1);
    newptr = newpath;
    pathbit = oldpwd;
    if (av[1][0] != '/' && pathbit[1] != 0)
        while (*pathbit)
            *newptr++ = *pathbit++;
    *newptr++ = '/';
    pathbit = _tokeniser(av[1], "/");
    while (pathbit != NULL)
    {
        if (pathbit[0] == '.' && pathbit[1] == '.' && pathbit[2] == 0)
        {
            newptr--;
            if (newptr != newpath)
                newptr--;
            while (*newptr != '/')
                newptr--;
            *newptr++ = '/';
        }
        else if (!(pathbit[0] == '.' && pathbit[1] == 0))
        {
            while (*pathbit)
                *newptr++ = *pathbit++;
            *newptr++ = '/';
        }
        pathbit = _tokeniser(NULL, "/");
    }
    if (newptr != newpath && newptr != newpath + 1)
        newptr--;
    *newptr = 0;
    ret = chdir(newpath);
    if (ret == 0)
    {
        _setenv("OLDPWD", oldpwd);
        free(oldpwd);
        _setenv("PWD", newpath);
        if (printpath)
            fprintstrs(1, newpath, "\n", NULL);
        free(newpath);
        return (0);
    }
    _errorhandle(": cd: can't cd to ");
    fprintstrs(STDERR_FILENO, av[1], "\n", NULL);
    free(oldpwd);
    free(newpath);
    return (ret);
}

/**
 * checkpath - checks the path
 * @av: arguments
 * Return: 1
 */
int checkpath(char *av[])
{
    char *path, *pathptr, *pathvar, *ptr, *pathenv = "PATH", *linect;
    int pathlen, cmdlen;

    for (ptr = av[0], cmdlen = 0; *ptr != 0; ptr++)
        cmdlen++;
    pathvar = _getenv(pathenv);
    if (pathvar != pathenv)
    {
        pathenv = pathvar;
        while (*pathvar != 0)
        {
#ifdef DEBUGMODE
            printf("in loop pathvar:%s:*pathvar:%c\n", pathvar, *pathvar);
#endif
            for (pathlen = 0, ptr = pathvar; *ptr != 0 && *ptr != ':'; ptr++)
                pathlen++;
            path = malloc(sizeof(char) * (cmdlen + pathlen + 2));
            if (path == NULL)
            {
                free(pathenv);
                return (-1);
            }
            pathptr = path;
            while (*pathvar != ':' && *pathvar != 0)
                *pathptr++ = *pathvar++;
            if (pathptr != path)
                *pathptr++ = '/';
            ptr = av[0];
            while (*ptr != 0)
                *pathptr++ = *ptr++;
            *pathptr = 0;
            if (!access(path, F_OK))
            {
#ifdef DEBUGMODE
                printf("Found path:%s\n", path);
#endif
                pathlen = cmdcall(av, path);
                free(path);
                free(pathenv);
                return (pathlen);
            }
            free(path);
            if (*pathvar == ':')
                pathvar++;
        }
    }
    linect = itos(linecount(0));
    path = getsvar("0");
    fprintstrs(2, path, ": ", linect, ": ", av[0], ": not found\n", NULL);
    free(path);
    free(linect);
    if (pathenv != pathvar)
        free(pathenv);
    return (127);
}
/**
 * cmdcall - calls commands
 * @av: arguments
 * @cmd: command
 * Return: retval
 */
int cmdcall(char *av[], char *cmd)
{
    pid_t command;
    int status;
    char *linect, *dolz;

    command = fork();
    if (command == 0)
    {
        if (execve(cmd, av, *(getenviron())) == -1)
        {
            if (!access(cmd, F_OK))
            {
                _errorhandle(NULL);
                exit(126);
            }
            else
            {
                linect = itos(linecount(0));
                dolz = getsvar("0");
                fprintstrs(2, dolz, ": ", linect, ": ", cmd, ": not found\n", NULL);
                free(dolz);
                free(linect);
                exit(127);
            }
            exit(1);
        }
    }
    else
    {
        wait(&status);
    }
    return (status);
}
/**
 * builtincall - calls builtin commands
 * @av: arguments
 * Return: retval
 */
int builtincall(char *av[])
{
    int retval = 0;
    char *retstr;

    if (av[0] == NULL)
        return (0);
    if (!_strcmp(av[0], "exit"))
    {
        if (av[1] != NULL)
            if (av[1][0] >= '0' && av[1][0] <= '9')
            {
                retval = _atoi(av[1]);
                exitcleanup(av);
                exit_hist();
                exit(retval);
            }
            else
            {
                _errorhandle(": exit: Illegal number: ");
                fprintstrs(STDERR_FILENO, av[1], "\n", NULL);
                retval = 2;
            }
        else
        {
            retstr = getsvar("?");
            retval = _atoi(retstr);
            free(retstr);
            exitcleanup(av);
            exit_hist();
            exit(retval);
        }
    }
    else if (!_strcmp(av[0], "cd"))
        retval = _cd(av);
    else if (!_strcmp(av[0], "history"))
        retval = print_hist();
    else if (!_strcmp(av[0], "help"))
        retval = help(av[1]);
    else if (!_strcmp(av[0], "env"))
        retval = _forenv();
    else if (!_strcmp(av[0], "setenv"))
        retval = _setenv(av[1], av[2]);
    else if (!_strcmp(av[0], "unsetenv"))
        retval = _unsetenv(av[1]);
    else if (!_strcmp(av[0], "alias"))
        retval = aliascmd(av);
    else if (!_strcmp(av[0], "unset"))
        retval = unsetsvar(av[1]);
    else if (!_strcmp(av[0], "unalias"))
        retval = unsetalias(av[1]);
    else if (av[0][0] != '/' &&
             !(av[0][0] == '.' && (av[0][1] == '/' ||
                                   (av[0][1] == '.' && av[0][2] == '/'))))
        retval = checkpath(av);
    else
        retval = cmdcall(av, av[0]);
    if (retval % 256 == 0)
        retval /= 256;
    retstr = itos(retval % 128);
    setsvar("?", retstr);
    free(retstr);
    return (retval);
}
