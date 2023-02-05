#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    SOMAR_FREQUENCIA = 1,
    NAO_SOMAR_FREQUENCIA = 2,
} FLAG_FREQUENCIA;

typedef struct Palavras *p_Palavras;

p_Palavras palavras_cria(char *, int);

int palavras_retorna_docs_quantidade(p_Palavras palavra);

int *palavras_retorna_docs_ids(p_Palavras palavra);

int *palavras_retorna_docs_frequencia(p_Palavras palavra);

double *palavras_retorna_docs_TFIFDs(p_Palavras palavra);

int compara_palavras(const void *a, const void *b);

void palavras_organiza_ordem(p_Palavras *, int);

int palavras_verifica_existencia(p_Palavras *vet, int qtd, char *palavra_alvo);

int palavras_get_indice(p_Palavras *, char *, int);

void palavras_registra_frequencia(p_Palavras, int);

void palavras_registra_indice(p_Palavras, int);

void palavras_imprime_informacoes(p_Palavras);

void palavras_preenche_IDF(p_Palavras, int);

double palavras_busca_e_preenche_TFIDF(p_Palavras, int);

/* Ordem de escrita:
idx de palavra                  - int
tam_vet                         - int
tam_palavra                     - int
palavra                         - char (* tam_palavra)
vet[tam_vet]{                   - IndicePalavras
----idx doc                     - int
----freq                        - int
----TFIDF                       - double
}
*/
void palavras_escrever_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal);

/* Ordem de leitura:
idx de palavra                  - int
tam_vet                         - int
tam_palavra                     - int
palavra                         - char (* tam_palavra)
vet[tam_vet]{                   - IndicePalavras
----idx doc                     - int
----freq                        - int
----TFIDF                       - double
}
*/
void palavras_le_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal);

void palavras_free(p_Palavras);