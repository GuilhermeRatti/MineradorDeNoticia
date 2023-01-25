#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Palavras.h"

typedef struct Documentos *p_Documentos;

p_Documentos documentos_cria(char*,char*,int);

void documentos_free(p_Documentos);

void documentos_imprime(p_Documentos);

p_Documentos documentos_preenche_tfidf(p_Documentos doc, int hash_palavra, double tfidf);

void documentos_escrever_arquivo_bin(FILE *arq, p_Documentos *vet_doc, int qtdDoc);

p_Documentos documentos_registra_frequencia(p_Documentos,int);