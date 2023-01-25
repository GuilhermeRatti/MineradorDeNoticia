#pragma once

typedef struct HashTable *p_HashTable;

p_HashTable hash_initialize_table();

int hash_get_index(char*);

p_HashTable hash_register_new_doc(p_HashTable,char*,char*);

p_HashTable hash_register_new_item(p_HashTable,char*, int);

void hash_print_amount_of_items(p_HashTable);

void hash_imprime_palavra(p_HashTable,char*);

p_HashTable hash_calcula_tfidf(p_HashTable);

void hash_free(p_HashTable);