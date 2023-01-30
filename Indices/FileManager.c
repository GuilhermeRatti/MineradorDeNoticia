#include "FileManager.h"
#include "HashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Classificadores.h"

p_HashTable manager_read_txt(FILE* arqEntrada, char* caminho_relativo, p_HashTable table, TIPO_LEITURA opcao)
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

        table = hash_register_new_doc(table, classe, diretorio_doc);

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
            table = hash_register_new_item(table,palavra,palavra_hash);
        }

        fclose(documento);
    }
    if(opcao == TRAIN)
    {
        table = hash_calcula_idf(table);
    }

    table = hash_calcula_tfidf(table,qtd_de_inicio);

    if(opcao == TRAIN)
        table = hash_calcula_centroides(table);

    return table;
}

p_HashTable manager_read_from_terminal(p_HashTable table, int *qtd_texto_digitados)
{
    char palavra[50],ch,nome[50],classe[4];
    int palavra_hash,i=0;

    int qtd_de_inicio = hash_retorna_qtd_doc(table);

    sprintf(nome,"Teste%d.txt",(*qtd_texto_digitados));
    (*qtd_texto_digitados)++;
    sprintf(classe,"tbd");
    table = hash_register_new_doc(table, classe, nome);

    printf("Digite um texto em uma unica linha para avalia-lo, e pressione ENTER quando tiver digitado tudo desejado.\n");

    while (1)
    {
        scanf("%c",&ch);
        
        if(ch==' ' || ch=='\n')
        {
            palavra[i] = '\0';  
            i=0;
            palavra_hash = hash_get_index(palavra);
            table = hash_register_new_item(table,palavra,palavra_hash);
        }
        else
        {
            palavra[i] = ch;
            i++;
        }

        if(ch=='\n')
        {
            break;
        }
    }

    table = hash_calcula_tfidf(table,qtd_de_inicio);

    Classificador modelo = classificadores_retorna_tipo(K_NEAREST_NEIGHBOURS);
    p_Documentos *vet; 
    int qtd = hash_get_dataset(table,&vet,K_NEAREST_NEIGHBOURS);

    modelo(vet,qtd,hash_retorna_doc(table,qtd_de_inicio));
    
    return table;
}