#include "shell.h"

char* all_commands[4000];

void getAllCommands(){
	FILE *fcomandos;
	char *atual;
	int i = 0;
	int len;
	fcomandos = fopen("all_commands","r");
	atual = (char*) malloc(sizeof(char)*50);
	while(fgets(atual,50,fcomandos)){
		len = strlen(atual);
		atual[len-1]='\0';
		all_commands[i] = atual;
		printf("%s\n",all_commands[i]);
		i++;
		atual = (char*) malloc(sizeof(char)*50);

		//	if(i == 10) break;
	}
	all_commands[i] = NULL;
	printf("%p\n",all_commands[i]);
	printf("todos os comandos: %d\n",i);
	fclose(fcomandos);
}


char* com_names[] = {
	"cd",
	"pwd",
	"exit",
	"pidof",
	"testestesteste",
	NULL
};

char** mysh_completion(const char *text, int start, int end){
	rl_attempted_completion_over = 0;
	return rl_completion_matches(text, mysh_generator);
}

char* mysh_generator(const char *text, int state){

	static int list_index, len;
	char *com;

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((com = all_commands[list_index++])) {
		if (strncmp(com, text, len) == 0) {
			return strdup(com);
		}
	}

	return NULL;
}

char* ler_linha(){
	char *linha = NULL;
	size_t buffer_size = 0;
	rl_attempted_completion_function = mysh_completion;
	//getline(&linha, &buffer_size,stdin);
	linha = readline(shellPrompt());


	if (linha != NULL && linha[0] != 0){
		add_history(linha);
	}

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
	//while (args[i] != NULL) {
	//	printf("\"%s\" ",args[i]);
	//	i++;
	//}
	//printf("\n");
	return args;
}

char *comandos[] = {
	"cd",
	"help",
	"exit",
	"pwd"
};

int ms_cd(char **args){
	int i = 1;
	char pasta_home[1024];
	char pasta_destino[1024];
	char arg[1024];
	char letra;
	strcpy(pasta_home,"/home/");
	strcat(pasta_home,getenv("LOGNAME"));

	if (!args[1]) {
		if (chdir(pasta_home)) {
			fprintf(stderr,"erro ao mudar diretorio\n");
		} 

	} else {

		if(args[1][0] == '~') {
			strcpy(pasta_destino,pasta_home);
			while (args[1][i]) {
				letra = args[1][i];
				strcat(pasta_destino,&letra);
				i++;
			}
			if (chdir(pasta_destino)) {
				fprintf(stderr,"erro ao mudar diretorio\n");
			}
		} else if (chdir(args[1])) {
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

int redirecionamento(char **args, int pos){
	int fd;
	int pid;
	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return 0;
	}
	if(pid==0){
		if (strcmp(args[pos],">") == 0){
			args[pos] = NULL;
			fd = open(args[pos+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			dup2(fd, STDOUT_FILENO); 
			close(fd);
		}else {
			args[pos] = NULL;
			fd = open(args[pos+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}

		char cwd[1024];
		setenv("parent",getcwd(cwd, 1024),1);
		if (execvp(args[0],args)==-1){
			printf("err");
			kill(getpid(),SIGTERM);
		} 
	}
	waitpid(pid,NULL,0);
	return 1;
}

int mysh_pipe(char **args, int pos){
	args[pos] = NULL;
	int pfds[2];
	int pid, pid2;
	pipe(pfds);
	if (pid = fork() == 0) {
		close(1);       
		dup(pfds[1]);   
		close(pfds[0]); 
		execvp(args[0], args);
	} else if (pid2 = fork() == 0){
		close(0);       
		dup(pfds[0]);   
		close(pfds[1]); 
		execvp(args[pos+1],&args[pos+1]);
	} else {
		waitpid(pid,NULL,0);
		waitpid(pid2,NULL,0);
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

	i = 0;
	while (args[i]) {
		if (strcmp(args[i],">") == 0 || strcmp(args[i],"<") == 0) {
			return redirecionamento(args,i);
		}
		if (strcmp(args[i],"|")== 0) {
			return mysh_pipe(args,i);
		}
		i++;
	}

	return mysh_launch(args);
}

char* shellPrompt(){
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	char cwd[1024];
	char *prompt = (char*) malloc(sizeof(char)*1024);
	//printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(cwd, sizeof(cwd)));
	strcpy(prompt, getenv("LOGNAME"));
	strcat(prompt, "@");
	strcat(prompt, hostn);
	strcat(prompt, ":");
	strcat(prompt, getcwd(cwd, sizeof(cwd)));
	strcat(prompt, "\n$ ");
	return prompt;
}

//TODO pipei

void mysh_loop(){
	char *linha;
	char **args;
	int status;
	FILE *fhistorico;
	fhistorico = fopen("mysh_history","a");
	int i;

	for (i = 0; i < 10; i++) {
		printf("-- %s\n",all_commands[i]);
	}

	do {
		char cwd[1024];

		// TODO auto complete aqui!!!!!! 
		linha = ler_linha();
		if(linha[0])
			fprintf(fhistorico,"%s\n",linha);
		args = split_linha(linha);
		status = mysh_exec(args);

		free(linha);
		free(args);
	} while (status);
	fclose(fhistorico);
}

