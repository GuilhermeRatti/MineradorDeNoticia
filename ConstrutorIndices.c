#include <stdio.h>
#include <stdlib.h>
#include "headers/FileManager.h"
#include "headers/HashTable.h"
#include <string.h>
#include <ctype.h>


int main(int argc, char const *argv[])
{
    // UMA SERIE DE VERIFICACOES DA VALIDADE DAS ENTRADAS DADAS
    if(argc < 3)
    {
        exit(printf("QUANTIDADE INSUFICIENTE DE INFORMACOES DISPONIBILIZADAS!\nINFORME O ARQUIVO TXT E O NOME DO ARQUIVO BINARIO DE SAIDA!!\n"));
    }

    if( argv[2][strlen(argv[2])-4]!='.' ||
        tolower(argv[2][strlen(argv[2])-3])!='b' || 
        tolower(argv[2][strlen(argv[2])-2])!='i' || 
        tolower(argv[2][strlen(argv[2])-1])!='n')
    {
        exit(printf("NOME DO ARQUIVO DE SAIDA ESTA FORMATADO INCORRETAMENTE!\nO FORMATO DEVE TERMINAR EM '.bin'!!\n"));
    }

    FILE *arqEntrada;

    arqEntrada = fopen(argv[1],"r");

    if(arqEntrada==NULL)
    {
        exit(printf("ARQUIVO TXT NAO ENCONTRADO!\n"));
    }

    /*
        PROBLEMA ENCONTRADO!!!
        
        No terminal, da-se como entrada o caminho relativo ate o arquivo train.txt q devera ser lido, como por ex: data/small/train.txt
        Dentro de cada linha desse train.txt, ele nos da mais um caminho para um documento que devera ser lido
        POREM ELE NAO MANTEM ESSE "CAMINHO RELATIVO", ou seja, nao seria possivel abri-lo do mesmo jeito que abrimos o train.txt
        
        Exemplo de um caminho dentro do train txt:
            train/0720112006inf.txt

        Exemplo do caminho DESEJADO:
            data/small/train/0720112006inf.txt

        EH NECESSARIO EXTRAIR ESSE "PREFIXO"/"CAMINHO RELATIVO" DE: data/small/ ÁRA INSERILO EM TODO NOVO CAMINHO QUE IREMOS LER, CASO CONTRARIO
        A LEITURA DE ARQUIVO DARA ***ERRRADO****
    */

    // PEQUENO ALGORITIMO QUE EXTRAI O CAMINHO DIGITADO PARA UTILIZA-LO POSTERIORMENTE
    char *caminho_relativo = strdup(argv[1]);

    int i=0;
    while (1)
    {
        if(caminho_relativo[strlen(caminho_relativo)-i]=='/')
            break;
        i++;
    }
    caminho_relativo[strlen(caminho_relativo) - i + 1]='\0';

    p_HashTable table = hash_initialize_table();
    manager_read_txt_train(arqEntrada, caminho_relativo, table, TRAIN);

    free(caminho_relativo);
    fclose(arqEntrada);
    
    hash_print_amount_of_items(table);

    //**Save das listas em arquivos binarios:

    hash_escrever_arquivo_bin(table, argv[2]);    

    hash_free(table);

    return 0;
}