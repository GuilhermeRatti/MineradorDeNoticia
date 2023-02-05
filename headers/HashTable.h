#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Classificadores.h"

typedef enum
{
    TRAIN = 1,
    TESTE = 2,
} TIPO_LEITURA;

typedef struct HashTable *p_HashTable;

/*
- Verifica a existencia de uma palavra a partir da sua possivel posicao na hash table.
- Ja que podem haver mais de uma palavra na mesma posicao na hash é necessário vericar se
a palavra esta contida dentro daquela posicao.
- A possivel posicao eh calculada usadno a funcao hash_get_index.
*/
int hash_palavra_verfica_existencia(p_HashTable table, char *palavra_alvo);

p_HashTable hash_initialize_table();

// Executa o algoritimo de calculo de um numero a partir de uma palavra
int hash_get_index(char *);

int hash_retorna_qtd_doc(p_HashTable);

void hash_register_new_doc(p_HashTable, p_Documentos);

void hash_register_new_item(p_HashTable, p_Documentos, char *, int);

void hash_print_amount_of_items(p_HashTable);

void hash_imprime_palavra(p_HashTable, char *);

void hash_imprime_documento(p_HashTable, int);

void hash_calcula_IDF(p_HashTable);

void hash_calcula_TFIDF_em_massa(p_HashTable, int);

void hash_calcula_TFIDF_do_doc(p_HashTable, p_Documentos, int, TIPO_LEITURA);

void hash_preenche_tfidf_docs(p_HashTable);

void hash_calcula_centroides(p_HashTable);

/*Ordem de escrita:
qtd_pal (total)                         - int
qtd_pal (na hash index)                 - int
OBS: escreve todos os indices da tabela hash, inclusive vazios
HashIndex{                              - HashIndex
----qtd_palavras_indice                 - int
----p_Palavras{chama a funcao 'palavras_escrever_arquivo_bin'}
}
qtd_doc                                 - int
p_Documentos{chama a funcao 'documentos_escrever_arquivo_bin'}
qtd_cls                                - int
centroides{chama a funcao 'documentos_escrever_arquivo_bin'}
*/
void hash_escrever_arquivo_bin(p_HashTable, const char *);

/*Ordem de leitura:
qtd_pal                                 - int
qtd_pal (na hash index)                 - int
OBS: le todos os indices da tabela hash, inclusive vazios
HashIndex{                              - HashIndex
----qtd_palavras_indice                 - int
----p_Palavras{chama a funcao 'palavras_le_arquivo_bin'}
}
qtd_doc                                 - int
p_Documentos{chama a funcao 'documentos_le_arquivo_bin'}
qtd_cls                                - int
centroides{chama a funcao 'documentos_le_arquivo_bin'}
*/
void hash_le_arquivo_bin(p_HashTable, const char *);

// Recebe um texto e mostra as 10 noticias que mias se parecem em questao de similaridade soma de TFIDF das palavras
void hash_buscar_noticias(p_HashTable, char *);

void hash_registra_noticia_do_terminal(p_HashTable, p_Documentos, char *);

char *hash_classificar_noticias(p_HashTable, p_Documentos, TIPOS_DISPONIVEIS, int);

/*Recebe uma palavra e mostra as 10 noticias em que ela aparece com mais frequência
e a frequência da palavra por classe.
*/
void hash_relatorio_palavras(p_HashTable, char *);

/*
Exibe os 10 documentos mais longos e os 10 mais curtos com o número de palavras e as respectivas classes.
*/
void hash_relatorio_documentos(p_HashTable);

void hash_preenche_tabela_classes(p_HashTable, char ***);

int hash_retorna_qtd_classes(p_HashTable);

void hash_free(p_HashTable);