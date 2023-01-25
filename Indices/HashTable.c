#include "HashTable.h"
#include "Palavras.h"
#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int SIZE_OF_TABLE = 23131;

typedef struct
{
    p_Palavras *vet_indice;
    int qtd_palavras_no_indice;
}HashIndex;

/*
    Apesar de haver uma lista de palavras e de documentos no hash,
    o hash so será calculado para as palavras; nao ha necessidade
    de calcular o hash dos documentos, visto a linearidade da leitura
    dos documentos e a impossibilidade de haver documentos duplicados
*/
struct HashTable
{
    HashIndex *pal_table;
    p_Documentos *doc_table;
    int doc_allcd;
    int qtd_doc;
    int pal_allcd;
    int qtd_pal;
};

p_HashTable hash_initialize_table()
{
    p_HashTable hash_table = (p_HashTable)calloc(1,sizeof(struct HashTable));
    hash_table->doc_allcd = 10;
    hash_table->pal_allcd = SIZE_OF_TABLE; //Um numero primo maior que 20000

    hash_table->doc_table = (p_Documentos*)calloc(hash_table->doc_allcd,sizeof(p_Documentos));
    hash_table->pal_table = (HashIndex*)calloc(hash_table->pal_allcd,sizeof(HashIndex));
    
    return hash_table;
}

int hash_get_index(char* palavra)
{
    int i, hash = 0, tam = strlen(palavra);

    for(i=0;i<tam;i++)
    {
        hash += palavra[i]*(i+1);
    }

    return hash%SIZE_OF_TABLE;
}

p_HashTable hash_register_new_doc(p_HashTable table, char* classe, char* diretorio)
{
    if(table->qtd_doc==table->doc_allcd)
    {
        table->doc_allcd*=2;
        table->doc_table = (p_Documentos*)realloc(table->doc_table,table->doc_allcd*sizeof(p_Documentos));
    }

    table->doc_table[table->qtd_doc] = documentos_cria(classe,diretorio,table->qtd_doc);
    table->qtd_doc++;

    return table;
}

p_HashTable hash_register_new_item(p_HashTable table, char* palavra, int p_index)
{
    p_Palavras p = palavras_cria(palavra, p_index);

    int doc = table->qtd_doc-1;
    int i,pos_no_indice = table->pal_table[p_index].qtd_palavras_no_indice;
    table->doc_table[doc] = documentos_registra_frequencia(table->doc_table[doc],p_index);
    
    for(i=0;i<pos_no_indice;i++)
    {
        int match = compara_palavras(&(table->pal_table[p_index].vet_indice[i]),&p);
        
        if(!match)
        {
            table->pal_table[p_index].vet_indice[i] = palavras_registra_frequencia(table->pal_table[p_index].vet_indice[i],doc);
            palavras_free(p);

            return table;
        }
    }

    table->qtd_pal++;
    table->pal_table[p_index].qtd_palavras_no_indice++;
    table->pal_table[p_index].vet_indice = (p_Palavras*)realloc(table->pal_table[p_index].vet_indice,
                                                                table->pal_table[p_index].qtd_palavras_no_indice*sizeof(p_Palavras));
    p = palavras_registra_frequencia(p,doc);
    table->pal_table[p_index].vet_indice[pos_no_indice] = p;

    return table;
}

void hash_print_amount_of_items(p_HashTable table)
{
    printf("QTD DE DOC E PALAVRA: %d %d\n", table->qtd_doc, table->qtd_pal);
}

void hash_imprime_palavra(p_HashTable table,char* palavra)
{
    int hash = hash_get_index(palavra),i,match;
    HashIndex desired_index = table->pal_table[hash];

    p_Palavras p = palavras_cria(palavra,0); // Encapsulando a palavra pra fazer comparacao

    for(i=0;i< desired_index.qtd_palavras_no_indice;i++)
    {
        match = compara_palavras(&(desired_index.vet_indice[i]),&p);

        if(!match)
        {
            palavras_imprime_informacoes(desired_index.vet_indice[i]);
            break;
        }
    }

    palavras_free(p);

    if(match!=0)
    {
        printf("PALAVRA '%s' NAO ENCONTRADA!!\nERA PRA ELA ESTAR REGISTRADA!?!?\n\n",palavra);
    }
}

p_HashTable hash_calcula_tfidf(p_HashTable table)
{
    int i,j,k,qtd_tfidf,qtd;

    for(i=0;i<table->pal_allcd;i++)
    {
        HashIndex id_desejado = table->pal_table[i];
        qtd = id_desejado.qtd_palavras_no_indice;
        for(j=0;j<qtd;j++)
        {
            double *vet_tfidf = (double*)malloc(sizeof(double));
            int *vet_docs = (int*)malloc(sizeof(int));

            id_desejado.vet_indice[j] = palavras_preenche_tfidf(id_desejado.vet_indice[j],table->qtd_doc, &vet_tfidf, &vet_docs,&qtd_tfidf);
            for(k=0;k<qtd_tfidf;k++)
            {
                table->doc_table[vet_docs[k]] = documentos_preenche_tfidf(table->doc_table[vet_docs[k]],
                                                                          i,
                                                                          vet_tfidf[k]);
            }
            free(vet_docs);
            free(vet_tfidf);
        }
    }

    return table;
}

void hash_free(p_HashTable table)
{
    int i,j;
    for(i=0;i<table->pal_allcd;i++)
    {
        for(j=0;j<table->pal_table[i].qtd_palavras_no_indice;j++)
        {
            palavras_free(table->pal_table[i].vet_indice[j]);
        }
        free(table->pal_table[i].vet_indice);
    }
    for(i=0;i<table->qtd_doc;i++)
    {
        documentos_free(table->doc_table[i]);
    }

    free(table->pal_table);
    free(table->doc_table);
    free(table);
}