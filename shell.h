#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARGS_SIZE 64
#define DELIM " \n"

char* ler_linha();
char** split_linha(char *linha);
int ms_cd(char **args);
int ms_help(char **args);
int ms_exit(char **args);
int ms_pwd(char **args);
int mysh_launch(char **args);
int mysh_exec(char **args);
void mysh_loop();

#endif
