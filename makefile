CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror
VFLAGS = --leak-check=full --show-reachable=yes --track-origins=yes
GFLAGS = -tui
EXEC = pruebas
OBJFILES = tp2.o heap.o lista.o strutil.o hash.o pila.o abb.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<
$(EXEC): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(EXEC)
all: $(EXEC)

run: all clear
	./$(EXEC) 400 < parametros.txt

valgrind: all clear
	valgrind $(VFLAGS) ./$(EXEC) 400 < parametros.txt
gdb: $(EXEC) clear
	gdb $(GFLAGS) ./$(EXEC)
free: $(OBJFILES)
	rm -f $(EXEC) *.o
clear:
	clear
