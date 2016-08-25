#include "shell.h"

int main(int argc, const char *argv[])
{
	getAllCommands();
	mysh_loop();
	return 0;
}
