#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct Documentos *p_Documentos;

p_Documentos documentos_cria(char *, char *, int);

int compara_classe_documentos(const void *, const void *);

void documentos_organiza_ordem(p_Documentos *, int);

void documentos_free(p_Documentos);

void documentos_imprime(p_Documentos);

char *documentos_retorna_classe(p_Documentos);

int documentos_retorna_id(p_Documentos doc);

int documentos_retorna_quantidade_palavras(p_Documentos doc);

int documentos_requisita_TFIDF(p_Documentos, char ***);

void documentos_preenche_TFIDF(p_Documentos, double *);

void documentos_preenche_centroide(p_Documentos, char **, double *, int);

void documentos_calcula_media_centroide(p_Documentos);

double documentos_calcula_cosseno(p_Documentos, p_Documentos);

/*Ordem de escrita:
idx de doc                      - int
tam_vet                         - int
tam_nome_doc                    - int
nome doc                        - char (* tam_nome_doc)
tam_nome_classe                 - int
classe                          - char (* tam_nome_classe)
vet[tam_vet]{                   - IndiceDocumentos
----tam_palavra                 - int
----palavra                     - char *
----freq                        - int
----TFIDF                       - double
}
*/
void documentos_escrever_arquivo_bin(FILE *, p_Documentos *, int);

/*Ordem de leitura:
idx de doc                      - int
tam_vet                         - int
tam_nome_doc                    - int
nome doc                        - char (* tam_nome_doc)
tam_nome_classe                 - int
classe                          - char (* tam_nome_classe)
vet[tam_vet]{                   - IndiceDocumentos
----tam_palavra                 - int
----palavra                     - char *
----freq                        - int
----TFIDF                       - double
}
*/
void documentos_le_arquivo_bin(FILE *, p_Documentos *, int);

void documentos_registra_frequencia(p_Documentos, char *);