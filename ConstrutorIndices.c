#include <stdio.h>
#include <stdlib.h>
#include "Documentos.h"
#include "Palavras.h"
#include <string.h>
#include <ctype.h>

int main(int argc, char const *argv[])
{
    p_Documentos *ListaDocumentos;  // VETOR DE DOCUMENTOS E SEUS INDICES A SEREM PREENCHIDOS
    p_Palavras *ListaPalavras;      // VETOR DE PALAVRAS E SEUS INDICES A SEREM PREENCHIDOS

    if(argc < 3)
    {
        exit(printf("QUANTIDADE INSUFICIENTE DE INFORMACOES DISPONIBILIZADAS!\nINFORME O ARQUIVO TXT E O NOME DO ARQUIVO BINARIO DE SAIDA!!\n"));
    }

    FILE *arqEntrada;

    arqEntrada = fopen(argv[1],"r");

    if(arqEntrada==NULL)
    {
        exit(printf("ARQUIVO TXT NAO ENCONTRADO!"));
    }

    if( argv[2][strlen(argv[2])-4]!='.' ||
        tolower(argv[2][strlen(argv[2])-3])!='b' || 
        tolower(argv[2][strlen(argv[2])-2])!='i' || 
        tolower(argv[2][strlen(argv[2])-1])!='n')
    {
        fclose(arqEntrada);
        exit(printf("NOME DO ARQUIVO DE SAIDA ESTA FORMATADO INCORRETAMENTE!\nO FORMATO DEVE TERMINAR EM '.bin'!!\n"));
    }

    /*
        FAZER FUNCOES QUE PERCORRAM OS DOCUMENTOS TXT E EXTRAIAM AS INFORMACOES DE FREQUENCIA, TFIDF ETC

        PRODUZIR ARQUIVOS BINARIOS DOS DOIS VETORES CONSTRUIDOS
    */

   fclose(arqEntrada);

    return 0;
}
