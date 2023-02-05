all: maker

CC = gcc
CODIGOS = codigos/Documentos.c codigos/Palavras.c codigos/FileManager.c codigos/HashTable.c codigos/Classificadores.c
HEADERS = headers/Documentos.h headers/Palavras.h headers/FileManager.h headers/HashTable.h headers/Classificadores.h
override CFLAGS += -ggdb3 -lm -Wall

libIndices.a: $(CODIGOS) $(HEADERS)
	rm -d objetos
	mkdir objetos
	gcc -c codigos/Documentos.c -o objetos/Documentos.o $(CFLAGS)
	gcc -c codigos/Classificadores.c -o objetos/Classificadores.o $(CFLAGS)
	gcc -c codigos/Palavras.c -o objetos/Palavras.o $(CFLAGS)
	gcc -c codigos/HashTable.c -o objetos/HashTable.o $(CFLAGS)
	gcc -c codigos/FileManager.c -o objetos/FileManager.o $(CFLAGS)
	ar -crs libIndices.a objetos/Documentos.o objetos/Palavras.o objetos/FileManager.o objetos/HashTable.o objetos/Classificadores.o

construtor: libIndices.a ConstrutorIndices.c
	$(CC) -o construtor ConstrutorIndices.c -I Indices -L . -lIndices $(CFLAGS)

principal: libIndices.a ProgramaPrincipal.c
	$(CC) -o principal ProgramaPrincipal.c -I Indices -L . -lIndices $(CFLAGS)

experimentos: libIndices.a Experimentos.c
	$(CC) -o experimentos Experimentos.c -I Indices -L . -lIndices $(CFLAGS)

run_c:
	./construtor data/medium-large/train.txt out_medium-large.bin

run_p:
	./principal out_medium-large.bin 100

run_e:
	./experimentos out_medium-large.bin data/medium-large/test.txt 1000 exp_medium-large.txt

val_c:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-construtor-out.txt -s ./construtor data/tiny/train.txt out_tiny.bin

val_p:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=val-principal-out.txt -s ./principal out_tiny.bin 1
	
val_e:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=val-experimentos-out.txt -s ./experimentos out_tiny.bin data/tiny/test.txt 100 exp_tiny.txt

clean:
	@rm -f main principal construtor experimentos *.a objetos/*.o *.ELF *.exe
