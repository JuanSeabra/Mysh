#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define ARGS_SIZE 64
#define DELIM " \n"


//lê e armazena a lista de todos os commandos possíveis do sistema
void getAllCommands();
//funções usadas pelo readline
char** mysh_completion(const char *text, int start, int end);
char* mysh_generator(const char *text, int state);
//Lê a linha passada pelo usuário
char* ler_linha();
//quebra a linha usando espaço como separador
char** split_linha(char *linha);
//comando cd nativo
int ms_cd(char **args);
//comando help nativo
int ms_help(char **args);
//comando exit nativo
int ms_exit(char **args);
//comando pwd nativo
int ms_pwd(char **args);
//executa comandos nativos do sistema usando exec
int mysh_launch(char **args);
//diferencia comandos nativos do sistema dos nativos do shell executando o correto para cada situação
int mysh_exec(char **args);
//exibe o prompt
char* shellPrompt();
//loop principal
void mysh_loop();

#endif
