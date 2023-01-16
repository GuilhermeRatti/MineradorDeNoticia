#include "Palavras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int IdxDocumento;
    int Frequencia;
    double TFIDF;
} IndicePalavras;

struct Palavras
{
    int tam_vet;
    char *palavra;
    IndicePalavras *vet;
};

int compara_palavras(const void *a, const void *b)
{
    p_Palavras p1 = *((p_Palavras*)a), p2 = *((p_Palavras*)b);
    
    return strcmp(p1->palavra,p2->palavra);
}

p_Palavras palavras_cria(p_Palavras*vet,int qtd,char* palavra)
{

    p_Palavras p = (p_Palavras)calloc(1,sizeof(struct Palavras));
    p->vet = (IndicePalavras*)calloc(0,sizeof(IndicePalavras));
    p->palavra = (char*)calloc(strlen(palavra),sizeof(char));
    strcpy(p->palavra,palavra);

    int existe = palavras_verifica_existencia(vet,qtd,p);
    if(existe)
    {
        free(p->palavra);
        free(p);
        return NULL;
    }

    return p;
}

int palavras_verifica_existencia(p_Palavras* vet,int qtd, p_Palavras p)
{
    p_Palavras* posicao = (p_Palavras*)bsearch(&p,vet,qtd,sizeof(p_Palavras),compara_palavras);

    if(posicao==NULL)
        return 0;
    else
        return 1;
}

void palavras_organiza_ordem(p_Palavras* vet,int qtd)
{
    qsort(vet,qtd,sizeof(p_Palavras),compara_palavras);
}