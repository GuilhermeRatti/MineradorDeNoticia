#include <stdio.h>
#include <stdlib.h>
#include "Documentos.h"
#include "Palavras.h"
#include <string.h>
#include <ctype.h>

void construtor_monta_listas(FILE *arqEntrada,
                             char* caminho_relativo,
                             p_Documentos *vetDoc, 
                             p_Palavras *vetPal, 
                             int *qtdDoc, 
                             int *qtdPal);

int main(int argc, char const *argv[])
{
    p_Documentos *ListaDocumentos = (p_Documentos*)calloc(0,sizeof(p_Documentos));  // VETOR DE DOCUMENTOS E SEUS INDICES A SEREM PREENCHIDOS
    p_Palavras *ListaPalavras = (p_Palavras*)calloc(0,sizeof(p_Palavras));          // VETOR DE PALAVRAS E SEUS INDICES A SEREM PREENCHIDOS
    int qtdDoc=0,qtdPal = 0;                                                        // VARIAVEIS Q ARMAZENAM A QTD DE DOCUMENTOS E PALAVRAS

    // UMA SERIE DE VERIFICACOES DA VALIDADE DAS ENTRADAS DADAS
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
    char *caminho_relativo = (char*)calloc(strlen(argv[1]),sizeof(char));
    strcpy(caminho_relativo,argv[1]);

    int i=0;
    while (1)
    {
        if(caminho_relativo[strlen(caminho_relativo)-i]=='/')
            break;
        
        i++;
    }
    caminho_relativo[i+1]='\0';

    construtor_monta_listas(arqEntrada,caminho_relativo,ListaDocumentos,ListaPalavras,&qtdDoc,&qtdPal);
    /*
        FAZER FUNCOES QUE PERCORRAM OS DOCUMENTOS TXT E EXTRAIAM AS INFORMACOES DE FREQUENCIA, TFIDF ETC

        PRODUZIR ARQUIVOS BINARIOS DOS DOIS VETORES CONSTRUIDOS
    */

   free(caminho_relativo);
   fclose(arqEntrada);
   
   return 0;
}

void construtor_monta_listas(FILE *arqEntrada,
                             char* caminho_relativo,
                             p_Documentos *vetDoc, 
                             p_Palavras *vetPal, 
                             int *qtdDoc, 
                             int *qtdPal)
{
    
    char linha[50];
    char caminho[50];
    char classe[5];
    char palavra[50];

    // Cada linha do arqEntrada contem o caminho para um documento e a classe do documento especificado

    while (fscanf(arqEntrada,"%[^\n]%*c",linha))
    {
        sscanf(linha,"%s %s",caminho,classe);
        vetDoc = (p_Documentos*)realloc(vetDoc,((*qtdDoc)+1)*sizeof(p_Documentos));

        vetDoc[(*qtdDoc)] = documentos_cria(caminho,classe);

        (*qtdDoc)++;

        FILE *documento;
        documento = fopen(strcat(caminho_relativo,caminho),"r");

        while (fscanf(documento,"%s",palavra))
        {
            p_Palavras res = palavras_cria(vetPal,(*qtdPal),palavra); // A FUNCAO RETORNARA NULL SE A PALAVRA JA EXISTIR NO DICIONARIO DE PALAVRAS REGISTRADAS

            if(res!=NULL) // SE A PALAVRA AINDA NAO TIVER SIDO REGISTRADA, ENTAO IRA REGISTRA-LA
            {
                vetPal = (p_Palavras*)realloc(vetPal,((*qtdPal)+1)*sizeof(p_Palavras));
                vetPal[(*qtdPal)] = res;
                (*qtdPal)++;

                // DEPOIS DE CADA PALAVRA ADICIONADA, DEVE-SE ORGANIZAR A LISTA EM ORDEM ALFABETICA PARA GARANTIR A INTEGRIDADE DO BSEARCH
                palavras_organiza_ordem(vetPal,(*qtdPal));
            }
        }

        fclose(documento);
    }
}