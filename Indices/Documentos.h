#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "Palavras.h"

typedef struct Documentos *p_Documentos;

p_Documentos documentos_cria(char*,char*,int);

void documentos_free(p_Documentos);

void documentos_imprime(p_Documentos);

p_Documentos documentos_preenche_tfidf(p_HashTable,p_Documentos);

void documentos_escrever_arquivo_bin(FILE*,p_Documentos*,int);

void documentos_le_arquivo_bin(FILE*,p_Documentos*,int);

p_Documentos documentos_registra_frequencia(p_Documentos,char*);