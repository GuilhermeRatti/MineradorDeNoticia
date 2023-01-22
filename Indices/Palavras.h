#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct Palavras *p_Palavras;

p_Palavras palavras_cria(p_Palavras*,int,char*);

void palavras_organiza_ordem(p_Palavras*,int);

int palavras_verifica_existencia(p_Palavras*,int,p_Palavras);

int palavras_get_indice(p_Palavras*,char*,int);

void palavras_registra_frequencia(p_Palavras*,int,int,int);

void palavras_registra_indice(p_Palavras,int);

void palavras_imprime_informacoes(p_Palavras *,int,char*);

void palavras_preenche_tfidf(p_Palavras *vet_pal, int qtdPal, int qtdDoc);

double palavras_busca_TFIDF(p_Palavras *vet_pal, int qtdPal, int idxDocPesq, int idxPalAlvo);

void palavras_free(p_Palavras);