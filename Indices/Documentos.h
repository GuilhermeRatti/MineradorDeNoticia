#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct Documentos *p_Documentos;

p_Documentos documentos_cria(char*,char*,int);

int compara_documentos(const void *, const void *);

void documentos_organiza_ordem(p_Documentos *, int);

void documentos_free(p_Documentos);

void documentos_imprime(p_Documentos);

void documentos_retorna_classe(p_Documentos,char**);

int documentos_requisita_idf(p_Documentos,char***);

p_Documentos documentos_preenche_TFIDF(p_Documentos, double*);

p_Documentos documentos_preenche_centroide(p_Documentos,p_Documentos);

p_Documentos documentos_calcula_media_centroide(p_Documentos);

double documentos_calcula_cosseno(p_Documentos,p_Documentos);

void documentos_escrever_arquivo_bin(FILE*,p_Documentos*,int);

void documentos_le_arquivo_bin(FILE*,p_Documentos*,int);

p_Documentos documentos_registra_frequencia(p_Documentos,char*);