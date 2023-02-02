#include "FileManager.h"
#include "HashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Classificadores.h"

void manager_read_txt(FILE* arqEntrada, char* caminho_relativo, p_HashTable table, TIPO_LEITURA opcao)
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
            hash_imprime_palavra(table,palavra);
        }

        fclose(documento);
    }
    if(opcao == TRAIN)
    {
        hash_calcula_IDF(table);
        hash_calcula_TFIDF_em_massa(table,qtd_de_inicio);
        hash_calcula_centroides(table);
    }

    if(opcao == TESTE)
    {
        int i/*,qtd_doc_a_ser_avaliado=hash_retorna_qtd_doc(table)-qtd_de_inicio*/;
        for(i=qtd_de_inicio;i<hash_retorna_qtd_doc(table);i++)
        {
            continue;
        }
    }
}