#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct Palavras *p_Palavras;

p_Palavras palavras_cria(char*,int);

int compara_palavras(const void *a, const void *b);

void palavras_organiza_ordem(p_Palavras*,int);

int palavras_verifica_existencia(p_Palavras*,int,p_Palavras);

int palavras_get_indice(p_Palavras*,char*,int);

p_Palavras palavras_registra_frequencia(p_Palavras,int);

void palavras_registra_indice(p_Palavras,int);

void palavras_imprime_informacoes(p_Palavras);

p_Palavras palavras_preenche_IDF(p_Palavras, int);

double palavras_preenche_TFIDF(p_Palavras*, int);

double palavras_preenche_e_retorna_TFIDF(p_Palavras *p, int doc);

void palavras_escrever_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal);

void palavras_le_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal);

void palavras_free(p_Palavras);