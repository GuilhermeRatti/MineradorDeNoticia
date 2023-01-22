#include <stdio.h>
#include <stdlib.h>
#include "Documentos.h"
#include "Palavras.h"
#include <string.h>
#include <ctype.h>

void construtor_monta_listas(FILE *arqEntrada,
                             char* caminho_relativo,
                             p_Documentos **vetDoc, 
                             p_Palavras **vetPal, 
                             int *qtdDoc, 
                             int *qtdPal,
                             int *doc_allcd,
                             int *pal_allcd);

void construtor_monta_frequencias(  FILE *arqEntrada,
                                    char* caminho_relativo,
                                    p_Documentos *vetDoc, 
                                    p_Palavras *vetPal,
                                    int qtd_pal);

void construtor_free_all(p_Documentos* lista_doc, 
                         p_Palavras* lista_pal, 
                         int qtd_doc, 
                         int qtd_pal);


int main(int argc, char const *argv[])
{
    int doc_allcd=10,pal_allcd=10;
    p_Documentos *ListaDocumentos = (p_Documentos*)calloc(doc_allcd,sizeof(p_Documentos));  // VETOR DE DOCUMENTOS E SEUS INDICES A SEREM PREENCHIDOS
    p_Palavras *ListaPalavras = (p_Palavras*)calloc(pal_allcd,sizeof(p_Palavras));          // VETOR DE PALAVRAS E SEUS INDICES A SEREM PREENCHIDOS
    int qtdDoc=0,qtdPal = 0;                                                                // VARIAVEIS Q ARMAZENAM A QTD DE DOCUMENTOS E PALAVRAS

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
    char *caminho_relativo = (char*)calloc(strlen(argv[1])+1,sizeof(char));
    strcpy(caminho_relativo,argv[1]);

    int i=0;
    while (1)
    {
        if(caminho_relativo[strlen(caminho_relativo)-i]=='/')
            break;
        
        i++;
    }
    caminho_relativo[i]='\0';

    construtor_monta_listas(arqEntrada,caminho_relativo,&ListaDocumentos,&ListaPalavras,&qtdDoc,&qtdPal, &doc_allcd, &pal_allcd);
    /*
        FAZER FUNCOES QUE PERCORRAM OS DOCUMENTOS TXT E EXTRAIAM AS INFORMACOES DE FREQUENCIA, TFIDF ETC

        PRODUZIR ARQUIVOS BINARIOS DOS DOIS VETORES CONSTRUIDOS
    */
    fclose(arqEntrada);

    arqEntrada = fopen(argv[1],"r");

    construtor_monta_frequencias(arqEntrada,caminho_relativo,ListaDocumentos,ListaPalavras,qtdPal);

    free(caminho_relativo);
    fclose(arqEntrada);

    printf("QTD DE DOC E PALAVRA: %d %d", qtdDoc, qtdPal);
    documentos_imprime(ListaDocumentos[0]);
    palavras_imprime_informacoes(ListaPalavras,qtdPal,"do");


    //**Formacao do TF-IDF:

    //Primeiro passo: Formacao do TF-IDF de cada palavra na lista de palavras
    palavras_preenche_tfidf(ListaPalavras, qtdPal, qtdDoc);

    //Segundo passo: Atribuição do TF-IDF das palavras na lista de documentos
    i=0;
    for ( i = 0; i < qtdDoc; i++)
    {
        p_Documentos docTfidf = ListaDocumentos[i];
        documentos_preenche_tfidf(&docTfidf, ListaPalavras, qtdPal);
    }
    

    //**Save das listas em arquivos binarios:




    construtor_free_all(ListaDocumentos, ListaPalavras, qtdDoc, qtdPal); 

    return 0;
}

void construtor_monta_listas(FILE *arqEntrada,
                             char* caminho_relativo,
                             p_Documentos **vetDoc, 
                             p_Palavras **vetPal, 
                             int *qtdDoc, 
                             int *qtdPal,
                             int *doc_allcd,
                             int *pal_allcd)
{
    
    char linha[100];
    char caminho[50];
    char classe[5];
    char palavra[50];

    // Cada linha do arqEntrada contem o caminho para um documento e a classe do documento especificado

    while (fscanf(arqEntrada,"%[^\n]\n",linha)!=EOF)
    {
        sscanf(linha,"%s %s",caminho,classe);

        char diretorio_doc[100];
        sprintf(diretorio_doc,"%s%s",caminho_relativo,caminho);

        if((*qtdDoc)==(*doc_allcd))
        {
            (*doc_allcd)*=2;
            (*vetDoc) = (p_Documentos*)realloc((*vetDoc),(*doc_allcd)*sizeof(p_Documentos));
        }
    
        (*vetDoc)[(*qtdDoc)] = documentos_cria(classe,caminho,(*qtdDoc));

        (*qtdDoc)++;

        FILE *documento;
        documento = fopen(diretorio_doc,"r");

        while (fscanf(documento,"%s",palavra)!=EOF)
        {
            p_Palavras resultado = palavras_cria((*vetPal),(*qtdPal),palavra); // A FUNCAO RETORNARA NULL SE A PALAVRA JA EXISTIR NO DICIONARIO DE PALAVRAS REGISTRADAS

            if(resultado!=NULL) // SE A PALAVRA AINDA NAO TIVER SIDO REGISTRADA, ENTAO IRA REGISTRA-LA
            {   
                if((*qtdPal) == (*pal_allcd))
                {
                    (*pal_allcd)*=2;
                    (*vetPal) = (p_Palavras*)realloc((*vetPal),(*pal_allcd)*sizeof(p_Palavras));
                }
                (*vetPal)[(*qtdPal)] = resultado;
                (*qtdPal)++;

                // DEPOIS DE CADA PALAVRA ADICIONADA, DEVE-SE ORGANIZAR A LISTA EM ORDEM ALFABETICA PARA GARANTIR A INTEGRIDADE DO BSEARCH
                palavras_organiza_ordem((*vetPal),(*qtdPal));
            }
        }
        
        int i;
        for(i=0;i<(*qtdPal);i++)
        {
            palavras_registra_indice((*vetPal)[i],i);
        }

        fclose(documento);
    }
}

void construtor_monta_frequencias(  FILE *arqEntrada,
                                    char* caminho_relativo,
                                    p_Documentos *vetDoc, 
                                    p_Palavras *vetPal,
                                    int qtd_pal)
{
    char linha[100],
         caminho[50],
         classe[5];

    int i=0;

    while (fscanf(arqEntrada,"%[^\n]\n",linha)!=EOF)
    {
        sscanf(linha,"%s %s",caminho,classe);
        char diretorio_doc[100];
        sprintf(diretorio_doc,"%s%s",caminho_relativo,caminho);

        vetDoc[i] = documentos_preenche_frequencia(vetDoc[i],vetPal, diretorio_doc, qtd_pal);
        i++;
    }   
}

void construtor_free_all(p_Documentos* lista_doc, p_Palavras* lista_pal, int qtd_doc, int qtd_pal)
{
    int i;
    for(i=0;i<qtd_doc;i++)
    {
        documentos_free(lista_doc[i]);
    }
    for(i=0;i<qtd_pal;i++)
    {
        palavras_free(lista_pal[i]);
    }

    free(lista_doc);
    free(lista_pal);
}