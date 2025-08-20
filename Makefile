all:	my_shell

my_shell:	my_shell.c builtins.c builtins.h
	gcc -ggdb -o my_shell my_shell.c builtins.c

clean:	
	rm my_shell


