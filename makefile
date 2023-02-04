all: maker

CC = gcc
CODIGOS = Indices/Documentos.c Indices/Palavras.c Indices/FileManager.c Indices/HashTable.c Indices/Classificadores.c
HEADERS = Indices/Documentos.h Indices/Palavras.h Indices/FileManager.h Indices/HashTable.h Indices/Classificadores.h
override CFLAGS += -ggdb3 -lm -Wall

libIndices.a: $(CODIGOS) $(HEADERS)
	gcc -c Indices/Documentos.c -o Indices/Documentos.o $(CFLAGS)
	gcc -c Indices/Classificadores.c -o Indices/Classificadores.o $(CFLAGS)
	gcc -c Indices/Palavras.c -o Indices/Palavras.o $(CFLAGS)
	gcc -c Indices/HashTable.c -o Indices/HashTable.o $(CFLAGS)
	gcc -c Indices/FileManager.c -o Indices/FileManager.o $(CFLAGS)
	ar -crs libIndices.a Indices/Documentos.o Indices/Palavras.o Indices/FileManager.o Indices/HashTable.o Indices/Classificadores.o

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
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=val-experimentos-out.txt -s ./experimentos out_medium-large.bin data/medium-large/test.txt 100 medium-large.txt

clean:
	@rm -f main principal construtor experimentos *.a Indices/*.o *.ELF *.exe
