#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char* ler_linha(){
	char *linha = NULL;
	size_t buffer_size = 0;
	getline(&linha, &buffer_size,stdin);
	printf("linha lida %s\n",linha); 
	return linha;
}

#define ARGS_SIZE 64
#define DELIM " \n"

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

	int i = 0;
	printf("args split: ");
	while (args[i] != NULL) {
		printf("\"%s\" ",args[i]);
		i++;
	}
	printf("\n");
	return args;
}

char *comandos[] = {
	"cd",
	"help",
	"exit",
	"echo"
};

int ms_cd(char **args){
	if (!args[1]) {
		fprintf(stderr,"esse comando espera argumentos!\n");
	} else {
		if (chdir(args[1])) {
			fprintf(stderr,"erro ao mudar diretorio\n");
		} 
	}
	return 1;
}

int ms_help(char **args){
	int i;
	printf("Mysh!\n");
	printf("comandos implementados por padrão:\n");
	for (i = 0; i < 3; i++) {
		printf("%s\n",comandos[i]);
	}
	return 1;
}

int ms_exit(char **args){
	return 0;
}
/*
int ms_echo(char **args){
	int i = 1;
	if(!args[1]) return 1;
	while (args[i]) {
		if (i == 1 && args[i+1]){
			if(args[1][0] != '\"'){
				fprintf(stderr,"sintaxe incorreta!");
			}
			printf("\s ",args[1][1]);
		} else if(args[i+1]) {
			printf("%s ",args[i]);
		} else {
			
		}
	}

}
*/
int (*comandos_func[]) (char**) = {
	&ms_cd,
	&ms_help,
	&ms_exit
//	&ms_echo
};

int mysh_exec(char **args){
	int i;

	if(!args[0]) return 1;

	for (i = 0; i < 3; i++) {
		if(strcmp(args[0],comandos[i]) == 0)
			return (*comandos_func[i])(args);
	}
}

void mysh_loop(){
	char *linha;
	char **args;
	int status;

	do {
		char cwd[1024];
		getcwd(cwd,sizeof(cwd));
		printf("%s $> ",cwd);
		linha = ler_linha();
		args = split_linha(linha);
		status = mysh_exec(args);

		free(linha);
		free(args);
	} while (status);
}

int main(int argc, const char *argv[])
{
	mysh_loop();
	return 0;
}
