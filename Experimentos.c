#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "FileManager.h"
#include "Classificadores.h"
#include "Documentos.h"
#include <string.h>
#include <ctype.h>

void experimento_gera_metricas(p_HashTable table,p_Documentos *vet_doc,int qtd,FILE*arq_out);

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

    int qtd_docs = manager_read_txt_test(arqEntrada,caminho_relativo,&docs_para_classificar,table);

    fclose(arqEntrada);

    FILE *arqOut = fopen(argv[4],"w");

    fprintf(arqOut,"RESULTADOS PARA O KNN DE K = 10\n\n");

    experimento_gera_metricas(table,docs_para_classificar,qtd_docs,arqOut,K_NEAREST_NEIGHBOURS,10);

    return 0;
}

void experimento_gera_metricas(p_HashTable table,p_Documentos *vet_doc,int qtd,FILE*arq_out,TIPOS_DISPONIVEIS opt, int k)
{
    int i,acertos=0,total=0;
    char *predicao;
    for(i=0;i<qtd;i++)
    {
        predicao = hash_classificar_noticias(table,vet_doc[i],opt,k);
        if(strcmp(predicao,documentos_retorna_classe(vet_doc[i]))==0)
        {
            acertos++;
        }

        total++;

    }
    fprintf(arq_out,"%.2f",(float)acertos/(float)total*100);
}