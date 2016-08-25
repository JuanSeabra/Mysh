all:
	gcc shell.c main.c -o mysh -g -lreadline

clean:
	rm mysh mysh_history

