#include "FileManager.h"
#include "HashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Classificadores.h"

void manager_read_txt_train(FILE* arqEntrada, char* caminho_relativo, p_HashTable table, TIPO_LEITURA opcao)
{
    char linha[100];
    char caminho[50];
    char classe[5];
    char palavra[50];
    int palavra_hash;

    int qtd_de_inicio = hash_retorna_qtd_doc(table);

    // Cada linha do arqEntrada contem o caminho para um documento e a classe do documento especificado

    while (fscanf(arqEntrada,"%[^\n]\n",linha)!=EOF)
    {
        sscanf(linha,"%s %s",caminho,classe);

        char diretorio_doc[100];
        sprintf(diretorio_doc,"%s%s",caminho_relativo,caminho);

        p_Documentos doc = documentos_cria(classe, diretorio_doc, hash_retorna_qtd_doc(table));

        hash_register_new_doc(table,doc);

        FILE *documento;
        documento = fopen(diretorio_doc,"r");
        
        if (documento == NULL)
        {
            fclose(arqEntrada);
            free(caminho_relativo);
            hash_free(table);
            exit(printf("ERRO: DOCUMENTO OU CAMINHO NAO ENCONTRADO!"));
        }
        

        while (fscanf(documento,"%s",palavra)!=EOF)
        {
            palavra_hash = hash_get_index(palavra);
            hash_register_new_item(table,doc,palavra,palavra_hash);
        }

        fclose(documento);
    }
    if(opcao == TRAIN)
    {
        hash_calcula_IDF(table);
        hash_calcula_TFIDF_em_massa(table,qtd_de_inicio);
        hash_calcula_centroides(table);
    }
}

int manager_read_txt_test(FILE* arqEntrada, char* caminho_relativo, p_Documentos **vet_doc, p_HashTable table)
{
    int qtd_docs = 0, docs_allcd = 1;

    char linha[100];
    char caminho[50];
    char classe[5];
    char palavra[50];
    int palavra_hash;

    while (fscanf(arqEntrada,"%[^\n]\n",linha)!=EOF)
    {

        sscanf(linha,"%s %s",caminho,classe);

        char diretorio_doc[100];
        sprintf(diretorio_doc,"%s%s",caminho_relativo,caminho);

        if(qtd_docs==docs_allcd)
        {
            docs_allcd*=2;
            (*vet_doc) = (p_Documentos*)realloc((*vet_doc),docs_allcd*sizeof(p_Documentos));
        }

        (*vet_doc)[qtd_docs] = documentos_cria(classe, diretorio_doc, hash_retorna_qtd_doc(table)+qtd_docs);

        FILE *documento;
        documento = fopen(diretorio_doc,"r");
        if (documento == NULL)
        {
            fclose(arqEntrada);
            free(caminho_relativo);
            hash_free(table);
            exit(printf("ERRO: DOCUMENTO OU CAMINHO NAO ENCONTRADO!"));
        }
        
        while (fscanf(documento,"%s",palavra)!=EOF)
        {
            palavra_hash = hash_get_index(palavra);
            hash_register_new_item(table,(*vet_doc)[qtd_docs],palavra,palavra_hash);
        }

        fclose(documento);

        hash_calcula_TFIDF_do_doc(table,(*vet_doc)[qtd_docs],hash_retorna_qtd_doc(table)+qtd_docs,TESTE);
        qtd_docs++;
    }
    
    return qtd_docs;
}