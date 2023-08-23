#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
/*all my prototypes*/
int shintmode(void);
int _forenv(void);
int builtincall(char *av[]);
int initsvars(int ac, char **av);
int sethist(char *cmd);
int print_hist(void);
int exit_hist(void);
int _putchar(char c);
int help(char *cmd);
void exitcleanup(char **av);
int _atoi(char *s);
char *_getpid(void);
int cmdcall(char *av[], char *path);
int parseargs(char **buf);
int errhandl(int status);
size_t _strlen(char *str);
int _unsetenv(char *name);
char **getallenv(void);
char *itos(int digits);
char *_strchr(char *s, char c);
char *getsvar(char *name);
int setsvar(char *name, char *val);
int unsetsvar(char *name);
void *_realloc(void *ptr, unsigned int old_size, unsigned int new_size);
char *_tokeniser(char *str, char *delim);
int _getline(char **lineptr, int fd);
char *strtokqe(char *str, char *delim, int escflags);
int fprintstrs(int fd, char *str, ...);
int _errorhandle(char *);
int linecount(int);
int _cd(char *av[]);
int aliascmd(char **av);
char *getalias(char *name);
char *forcopy(char *dest, char *src);
int _strcmp(char *, char *);
char *_strdup(char *str);
char *_strcat(char *a, char *b);
char ***getenviron(void);
int setallenv(char **environ, char *add);
char *_getenv(char *avzero);
int _setenv(char *name, char *val);
int unsetalias(char *name);

/**
 * struct State - singly linked list
 * @cmd: command line entered
 * @next: pointer to the next node
 * Description: singly linked list node structure for History
 */

typedef struct State
{
    char *cmd;
    struct State *next;
} State;

/**
 * struct Shell - Shell list node
 * @name: name of variable
 * @val: value of variable
 * @next: next variable in the list
 */
typedef struct Shell
{
    char *name;
    char *val;
    struct Shell *next;
} Shell;
Shell **getspecial(void);
Shell **getvars(void);

/**
 * struct alias - struct for alias function
 * @alias - name of the struct
 * @name: name of the alias value
 * @val: value for the alias
 * @next: pointer to next node
 *
 */
typedef struct alias
{
    char *name;
    char *val;
    struct alias *next;
} alias;

#endif
