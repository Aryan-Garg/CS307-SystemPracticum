all: shell.o dining.o

shell.o: shell_updated.c
	gcc -Wall shell_updated.c -o shell.o -lreadline

dining.o: dining.c
	gcc -o dining.o dining.c -lpthread
	
.PHONY: clean

clean:
	rm -rf *.o
