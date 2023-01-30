#include <stdio.h>
#include <stdlib.h>
#include "FileManager.h"
#include "HashTable.h"
#include <string.h>
#include <ctype.h>

int main(int argc, char const *argv[])
{
    if(argc < 2)
    {
        exit(printf("QUANTIDADE INSUFICIENTE DE INFORMACOES DISPONIBILIZADAS!\nINFORME O ARQUIVO BIN E O NUMERO K DE VIZINHOS!!\n"));
    }

    if( argv[1][strlen(argv[1])-4]!='.' ||
        tolower(argv[1][strlen(argv[1])-3])!='b' || 
        tolower(argv[1][strlen(argv[1])-2])!='i' || 
        tolower(argv[1][strlen(argv[1])-1])!='n')
    {
        exit(printf("NOME DO ARQUIVO DE ENTRADA ESTA FORMATADO INCORRETAMENTE!\nO FORMATO DEVE TERMINAR EM '.bin'!!\n"));
    }

    //**Parte da leitura do arquivo.bin para preenchimento da tabela HASH:

    p_HashTable table = hash_initialize_table();

    FILE *bin = fopen(argv[1], "rb");   

    if (bin == NULL)
    {
        exit(printf("ERRO: ARQUIVO BINARIO NAO ENCONTRADO"));
    }    

    table = hash_le_arquivo_bin(table, bin);

    fclose(bin);
    int holder = 1;
    table = manager_read_from_terminal(table,&holder);

    hash_imprime_palavra(table,"judiciario");
    hash_imprime_documento(table,15);

    hash_free(table);

    

}