#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "FileManager.h"
#include "Classificadores.h"
#include "Documentos.h"
#include <string.h>
#include <ctype.h>

void experimento_gera_metricas(p_HashTable ,p_Documentos *,int ,FILE*,TIPOS_DISPONIVEIS , int ,char**);
int experimento_classe_para_inteiro(char*,char**,int);
void experimento_matriz_confusao_free(int**,int);
void experimento_imprime_matriz_confusao(FILE*, int**, int, char**);
void experimento_free(p_HashTable,char**,p_Documentos*,int);

int main(int argc, char const *argv[])
{
    // UMA SERIE DE VERIFICACOES DA VALIDADE DAS ENTRADAS DADAS
    if(argc < 5)
    {
        exit(printf("QUANTIDADE INSUFICIENTE DE INFORMACOES DISPONIBILIZADAS!\nINFORME O ARQUIVO BINARIO E TXT DE ENTRADA, QTD DE K VIZINHOS E TXT DE SAIDA!!\n"));
    }

    if( argv[1][strlen(argv[1])-4]!='.' ||
        tolower(argv[1][strlen(argv[1])-3])!='b' || 
        tolower(argv[1][strlen(argv[1])-2])!='i' || 
        tolower(argv[1][strlen(argv[1])-1])!='n')
    {
        exit(printf("NOME DO ARQUIVO BINARIO DE ENTRADA ESTA FORMATADO INCORRETAMENTE!\nO FORMATO DEVE TERMINAR EM '.bin'!!\n"));
    }

    if( argv[4][strlen(argv[4])-4]!='.' ||
        tolower(argv[4][strlen(argv[4])-3])!='t' || 
        tolower(argv[4][strlen(argv[4])-2])!='x' || 
        tolower(argv[4][strlen(argv[4])-1])!='t')
    {
        exit(printf("NOME DO ARQUIVO TXT DE SAIDA ESTA FORMATADO INCORRETAMENTE!\nCONFIRA O NOME E SE O FORMATO TERMINA EM '.txt'!!\n"));
    }

    if (!atoi(argv[3]))
    {
        exit(printf("ERRO: Um numero deve ser informado como valor de K.\n"));
    }

    FILE *arqEntrada;

    arqEntrada = fopen(argv[2],"r");

    if(arqEntrada==NULL)
    {
        exit(printf("ARQUIVO TXT NAO ENCONTRADO!\n"));
    }

    //**Parte da leitura do arquivo.bin para preenchimento da tabela HASH:

    p_HashTable table = hash_initialize_table();
    hash_le_arquivo_bin(table, argv[1]);
    p_Documentos *docs_para_classificar = (p_Documentos*)calloc(1,sizeof(p_Documentos));
    
    char *caminho_relativo = strdup(argv[2]);
    
    int i=0;
    while (1)
    {
        if(caminho_relativo[strlen(caminho_relativo)-i]=='/')
            break;
        i++;
    }
    caminho_relativo[strlen(caminho_relativo) - i + 1]='\0';

    int k = atoi(argv[3]);
    int qtd_docs = manager_read_txt_test(arqEntrada,caminho_relativo,&docs_para_classificar,table);
    fclose(arqEntrada);

    char **tabela_classes = (char**)calloc(1,sizeof(char*));
    hash_preenche_tabela_classes(table,&tabela_classes);

    FILE *arqOut = fopen(argv[4],"w");
    fprintf(arqOut,"\nRESULTADOS PARA O KNN DE K = %d\n\n",k);
    experimento_gera_metricas(table,docs_para_classificar,qtd_docs,arqOut,K_NEAREST_NEIGHBOURS,k,tabela_classes);
    fprintf(arqOut,"\nRESULTADOS PARA O CENTROIDE MAIS PROXIMO\n\n");
    experimento_gera_metricas(table,docs_para_classificar,qtd_docs,arqOut,CENTROIDE_MAIS_PROXIMA,k,tabela_classes);

    fclose(arqOut);
    free(caminho_relativo);
    experimento_free(table,tabela_classes,docs_para_classificar,qtd_docs);

    return 0;
}

void experimento_gera_metricas(p_HashTable table,p_Documentos *vet_doc,int qtd,FILE*arq_out,TIPOS_DISPONIVEIS opt, int k,char**tabela_classes)
{
    int i,acertos=0,total=0,lin,col;
    int qtd_classes = hash_retorna_qtd_classes(table);
    int **matriz_confusao = (int**)calloc(qtd_classes,sizeof(int*));
    for(i=0;i<qtd_classes;i++)
        matriz_confusao[i]=(int*)calloc(qtd_classes,sizeof(int));

    char *predicao;
    for(i=0;i<qtd;i++)
    {
        predicao = hash_classificar_noticias(table,vet_doc[i],opt,k);
        if(strcmp(predicao,documentos_retorna_classe(vet_doc[i]))==0)
        {
            acertos++;
        }
        lin = experimento_classe_para_inteiro(documentos_retorna_classe(vet_doc[i]),tabela_classes,qtd_classes);
        col = experimento_classe_para_inteiro(predicao,tabela_classes,qtd_classes);

        matriz_confusao[lin][col]++;

        total++;
    }
    fprintf(arq_out,"ACURACIA: %.2f\n",(float)acertos/(float)total*100);
    experimento_imprime_matriz_confusao(arq_out,matriz_confusao,qtd_classes,tabela_classes);
    experimento_matriz_confusao_free(matriz_confusao,qtd_classes);
}

int experimento_classe_para_inteiro(char*classe,char**tabela_classes, int qtd_classes)
{
    int i;
    for(i=0;i<qtd_classes;i++)
    {
        if(!strcmp(tabela_classes[i],classe))
            return i;
    }

    return -1; //esse caso eh pra quebrar o programa mesmo
}

void experimento_matriz_confusao_free(int** matriz,int dimensoes)
{
    int i;
    for(i=0;i<dimensoes;i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}

void experimento_imprime_matriz_confusao(FILE* arqOut, int** matriz, int dimensoes,char**tabela_classes)
{
    int i,j;
    fprintf(arqOut,"\n\t");
    for(i=0;i<dimensoes;i++)
    {
        fprintf(arqOut,"\t%s", tabela_classes[i]);
    }
    for(i=0;i<dimensoes;i++)
    {
        fprintf(arqOut,"\n%s\t",tabela_classes[i]);
        for(j=0;j<dimensoes;j++)
        {
            fprintf(arqOut,"\t%d",matriz[i][j]);
        }
    }
}

void experimento_free(p_HashTable table, char**tabela_classes, p_Documentos*vet_docs, int qtd_docs)
{
    int i;
    for(i=0;i<hash_retorna_qtd_classes(table);i++)
    {
        free(tabela_classes[i]);
    }
    free(tabela_classes);

    hash_free(table);
    for(i=0;i<qtd_docs;i++)
    {
        documentos_free(vet_docs[i]);
    }
    free(vet_docs);
}