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
	$(CC) -o builder ConstrutorIndices.c -I Indices -L . -lIndices $(CFLAGS)

run_main:
	./main data 

run_b:
	./builder data/tiny/train.txt out.bin

clean:
	@rm -f main builder *.a Indices/*.o *.txt *.bin

val_b:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt -s ./builder data/tiny/train.txt out.bin