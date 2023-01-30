#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct HashTable *p_HashTable;


int hash_palavra_verfica_existencia(p_HashTable table, char *palavra_alvo);

p_HashTable hash_initialize_table();

int hash_get_index(char*);

int hash_retorna_qtd_doc(p_HashTable);

p_HashTable hash_register_new_doc(p_HashTable,char*,char*);

p_HashTable hash_register_new_item(p_HashTable,char*, int);

void hash_print_amount_of_items(p_HashTable);

void hash_imprime_palavra(p_HashTable,char*);

void hash_imprime_documento(p_HashTable,int);

p_HashTable hash_calcula_IDF(p_HashTable);

p_HashTable hash_calcula_TFIDF(p_HashTable,int);

p_HashTable hash_preenche_tfidf_docs(p_HashTable);

p_HashTable hash_calcula_centroides(p_HashTable);

void hash_escrever_arquivo_bin(p_HashTable, const char *arq);

void hash_le_arquivo_bin(p_HashTable, const char *arq);

void hash_buscar_noticias(p_HashTable table, char *texto);

void hash_relatorio_noticias(p_HashTable table, char *palavra_relatorio);

void hash_relatorio_documentos(p_HashTable table);

void hash_free(p_HashTable);