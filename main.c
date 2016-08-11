#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* ler_linha(){
	char *linha = NULL;
	size_t buffer_size = 0;
	getline(&linha, &buffer_size,stdin);
	return linha;
}

#define ARGS_SIZE 64
#define DELIM " \t\r\n\a"

char** split_linha(char *linha){
	int posicao = 0;
	int tam = ARGS_SIZE;
	char **args = malloc(sizeof(char*)*tam);
	char *arg;

	if(!args){
		fprintf(stderr, "erro ao alocar!!");
		exit(EXIT_FAILURE);
	}

	arg = strtok(linha,DELIM);
	while (arg) {
		args[posicao] = arg;
		posicao++;

		if (posicao == tam) {
			tam += ARGS_SIZE;
			args = realloc(args, tam*sizeof(char*));
			if (!args) {
				fprintf(stderr, "erro ao alocar!!");
				exit(EXIT_FAILURE);
			}
		}
		arg = strtok(NULL,DELIM);
	}
	args[posicao] = NULL;
	return args;
}

void mysh_loop(){
	char *linha;
	char **args;
	int status;

	do {
		printf("$> ");
		linha = ler_linha();
		args = split_linha(linha);
		//status = mysh_exec(args);

		free(linha);
		free(args);
	} while (status);
}

int main(int argc, const char *argv[])
{
	mysh_loop();
	return 0;
}
