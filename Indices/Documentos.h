#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Palavras.h"

typedef struct Documentos *p_Documentos;

p_Documentos documentos_cria(char*,char*,int);

void documentos_free(p_Documentos);

void documentos_imprime(p_Documentos);

void documentos_preenche_tfidf(p_Documentos *doc,p_Palavras *vet_pal,int qtdPal);

p_Documentos documentos_preenche_frequencia(p_Documentos,p_Palavras* vet_pal,char*,int);