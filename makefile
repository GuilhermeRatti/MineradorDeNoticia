all: maker

CC = gcc
CODIGOS = Indices/Documentos.c Indices/Palavras.c
HEADERS = Indices/Documentos.h Indices/Palavras.h
override CFLAGS += -ggdb3 -lm -Wall

libIndices.a: $(CODIGOS) $(HEADERS)
	gcc -c Indices/Documentos.c -o Indices/Documentos.o $(CFLAGS)
	gcc -c Indices/Palavras.c -o Indices/Palavras.o $(CFLAGS)
	ar -crs libIndices.a Indices/Documentos.o Indices/Palavras.o

maker: libIndices.a ProgramaPrincipal.c
	$(CC) -o main ProgramaPrincipal.c -I Indices -L . -lIndices $(CFLAGS)

builder: libIndices.a ConstrutorIndices.c
	$(CC) -o main ConstrutorIndices.c -I Indices -L . -lIndices $(CFLAGS)

run_main:
	./main data

run_builder:
	./builder 

clean:
	@rm -f main *.a spotify/*.o *.txt *.bin

val:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt -s ./main data