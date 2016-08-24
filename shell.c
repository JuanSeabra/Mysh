#include "shell.h"

char* ler_linha(){
	char *linha = NULL;
	size_t buffer_size = 0;
	getline(&linha, &buffer_size,stdin);
	//printf("linha lida %s\n",linha); 
	return linha;
}

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
	//printf("args split: ");
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
	"pwd"
};

int ms_cd(char **args){
	//TODO mapear /home/user em ~
	//TODO cd em branco volta pra home
	if (!args[1]) {
		fprintf(stderr,"esse comando espera argumentos!\n");
	} else {
		//TODO args[1][1] == '~'
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
	for (i = 0; i < 4; i++) {
		printf("%s\n",comandos[i]);
	}
	return 1;
}

int ms_exit(char **args){
	return 0;
}

int ms_pwd(char **args){
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	printf("%s\n",cwd);
	return 1;
}

int (*comandos_func[]) (char**) = {
	&ms_cd,
	&ms_help,
	&ms_exit,
	&ms_pwd
};


int mysh_launch(char **args){
	pid_t pid, wpid;
	int status;

	pid = fork();
	if(!pid){
		//filho
		if(execvp(args[0],args) == -1){
			perror("mysh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0){
		//erro
		perror("mysh");
	} else {
		//pai
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int mysh_exec(char **args){
	int i;

	if(!args[0]) return 1;

	for (i = 0; i < 4; i++) {
		if(strcmp(args[0],comandos[i]) == 0)
			return (*comandos_func[i])(args);
	}
	return mysh_launch(args);
}



void shellPrompt(){
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	char cwd[1024];
	printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(cwd, sizeof(cwd)));
}

void mysh_loop(){
	char *linha;
	char **args;
	int status;
	FILE *fhistorico;
	fhistorico = fopen("mysh_history","w+");

	do {
		char cwd[1024];
		shellPrompt();
		// TODO auto complete aqui!!!!!! 
		linha = ler_linha();
		fprintf(fhistorico,"%s",linha);
		args = split_linha(linha);
		status = mysh_exec(args);

		free(linha);
		free(args);
	} while (status);
}

