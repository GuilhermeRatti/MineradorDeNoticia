#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Classificadores.h"

typedef enum
{
    TRAIN = 1,
    TESTE = 2,
}TIPO_LEITURA;

typedef struct HashTable *p_HashTable;

int hash_palavra_verfica_existencia(p_HashTable table, char *palavra_alvo);

p_HashTable hash_initialize_table();

int hash_get_index(char*);

int hash_retorna_qtd_doc(p_HashTable);

void hash_register_new_doc(p_HashTable,p_Documentos);

void hash_register_new_item(p_HashTable,p_Documentos,char*, int);

void hash_print_amount_of_items(p_HashTable);

void hash_imprime_palavra(p_HashTable,char*);

void hash_imprime_documento(p_HashTable,int);

void hash_calcula_IDF(p_HashTable);

void hash_calcula_TFIDF_em_massa(p_HashTable,int);

void hash_calcula_TFIDF_do_doc(p_HashTable,p_Documentos, int,TIPO_LEITURA);

void hash_preenche_tfidf_docs(p_HashTable);

void hash_calcula_centroides(p_HashTable);

void hash_escrever_arquivo_bin(p_HashTable, const char *);

void hash_le_arquivo_bin(p_HashTable, const char *);

void hash_buscar_noticias(p_HashTable, char *);

void hash_registra_noticia_do_terminal(p_HashTable, p_Documentos, char*);

char* hash_classificar_noticias(p_HashTable, p_Documentos, TIPOS_DISPONIVEIS, int);

void hash_relatorio_palavras(p_HashTable, char *);

void hash_relatorio_documentos(p_HashTable);

void hash_free(p_HashTable);