#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct Palavras *p_Palavras;

p_Palavras palavras_cria(char*,int);

int palavras_retorna_docs_quantidade(p_Palavras palavra);

int *palavras_retorna_docs_ids(p_Palavras palavra);

int *palavras_retorna_docs_frequencia(p_Palavras palavra);

double *palavras_retorna_docs_TFIFDs(p_Palavras palavra);

int compara_palavras(const void *a, const void *b);

void palavras_organiza_ordem(p_Palavras*,int);

int palavras_verifica_existencia(p_Palavras* vet,int qtd, char *palavra_alvo);

int palavras_get_indice(p_Palavras*,char*,int);

p_Palavras palavras_registra_frequencia(p_Palavras,int);

void palavras_registra_indice(p_Palavras,int);

void palavras_imprime_informacoes(p_Palavras);

p_Palavras palavras_preenche_tfidf(p_Palavras p, int qtdDoc, double **vet_tfidf, int **vet_docs, int *qtd_tfidf);

double palavras_busca_TFIDF(p_Palavras,int);

void palavras_escrever_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal);

void palavras_le_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal);

void palavras_free(p_Palavras);