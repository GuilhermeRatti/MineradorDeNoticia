#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct HashTable *p_HashTable;

p_HashTable hash_initialize_table();

int hash_get_index(char*);

p_HashTable hash_register_new_doc(p_HashTable,char*,char*);

p_HashTable hash_register_new_item(p_HashTable,char*, int);

void hash_print_amount_of_items(p_HashTable);

void hash_imprime_palavra(p_HashTable,char*);

void hash_imprime_documento(p_HashTable,int);

p_HashTable hash_calcula_tfidf(p_HashTable);

p_HashTable hash_preenche_tfidf_docs(p_HashTable);

double hash_return_tfidf(p_HashTable,int,char*);

void hash_escrever_arquivo_bin(p_HashTable, FILE *arq);

void hash_le_arquivo_bin(p_HashTable, FILE *arq);

void hash_free(p_HashTable);