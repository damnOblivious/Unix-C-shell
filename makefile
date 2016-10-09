CC=gcc
DEPS = headers.h
OBJ = functions.o basics.o  redirection.o cshell.o 

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

cshell: $(OBJ)
		gcc -o $@ $^ $(CFLAGS)
