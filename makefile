all: maker

CC = gcc
CODIGOS = Indices/Documentos.c Indices/Palavras.c Indices/FileManager.c Indices/HashTable.c Indices/Classificadores.c
HEADERS = Indices/Documentos.h Indices/Palavras.h Indices/FileManager.h Indices/HashTable.h Indices/Classificadores.h
override CFLAGS += -ggdb3 -lm -Wall

libIndices.a: $(CODIGOS) $(HEADERS)
	gcc -c Indices/Documentos.c -o Indices/Documentos.o $(CFLAGS)
	gcc -c Indices/Palavras.c -o Indices/Palavras.o $(CFLAGS)
	gcc -c Indices/HashTable.c -o Indices/HashTable.o $(CFLAGS)
	gcc -c Indices/FileManager.c -o Indices/FileManager.o $(CFLAGS)
	gcc -c Indices/Classificadores.c -o Indices/Classificadores.o $(CFLAGS)
	ar -crs libIndices.a Indices/Documentos.o Indices/Palavras.o Indices/FileManager.o Indices/HashTable.o Indices/Classificadores.o

builder: libIndices.a ConstrutorIndices.c
	$(CC) -o builder ConstrutorIndices.c -I Indices -L . -lIndices $(CFLAGS)

principal: libIndices.a ProgramaPrincipal.c
	$(CC) -o principal ProgramaPrincipal.c -I Indices -L . -lIndices $(CFLAGS)

run_b:
	./builder data/tiny/train.txt out.bin

run_p:
	./principal out.bin 10

val_b:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt -s ./builder data/tiny/train.txt out.bin

val_p:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt -s ./principal out.bin 10
	
clean:
	@rm -f main builder *.a Indices/*.o *.txt *.bin
